#include "app.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include "export.h"
#include "input.h"
#include "pet.h"
#include "platform.h"
#include "render.h"
#include "theme.h"
#include "utf8.h"
#include "version.h"
#include "view.h"
#include "viewkind.h"

#define APP_PADDING 10
#define APP_WINDOW_TITLE "Codename Hello"
#define APP_REPLY_BASE_MS 220
#define APP_REPLY_PER_CHAR_MS 22
#define APP_REPLY_MAX_MS 900
#define APP_CARET_BLINK_MS 500
#define APP_SCROLL_STEP 3
#define APP_STATUS_MAX 192

typedef struct {
    Config cfg;

    /* Своя копия темы: встроенные неизменяемы, а /color правит копию. */
    Theme theme_storage;
    const Theme *theme;

    Chat *chat;
    View view;
    Input input;
    Pet pet;

    TTF_Font *font;
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width;
    int height;
    int line_h;
    int visible_rows;
    int text_bottom;

    bool running;
    bool screenshot;
    bool fullscreen;

    /* Выделение мышью: индексы визуальных строк включительно. */
    bool sel_active;
    int sel_anchor;
    int sel_end;

    uint32_t pending_at; /* 0 — ответа в очереди нет */
    char *pending_input;

    uint64_t messages;
    uint64_t frames;
    uint32_t start_ticks;
    uint32_t last_frame_ticks;
    double fps;

    const char *config_path;
    char pet_path[CONFIG_PATH_MAX];
    char export_path[CONFIG_PATH_MAX];
} App;

static char *trim_dup(const char *text)
{
    if (text == NULL)
        return NULL;

    const char *start = text;
    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n')
        ++start;

    size_t len = strlen(start);
    while (len > 0) {
        char c = start[len - 1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        --len;
    }
    return utf8_dup_range(start, len);
}

static void add_line(App *app, const char *text, ViewKind kind)
{
    view_add(&app->view, text, kind, app->font, app->renderer, app->theme,
             app->width - 2 * APP_PADDING);
}

static void addf(App *app, ViewKind kind, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    add_line(app, buffer, kind);
}

static void update_size(App *app)
{
    int w = 0, h = 0;
    if (!SDL_GetRenderOutputSize(app->renderer, &w, &h) || w <= 0 || h <= 0)
        SDL_GetWindowSize(app->window, &w, &h);
    app->width = w;
    app->height = h;
}

static void compute_layout(App *app)
{
    app->line_h = TTF_GetFontLineSkip(app->font);

    int status_h = app->line_h;
    int input_h = app->line_h;
    int area_top = APP_PADDING;
    int area_bottom = app->height - APP_PADDING - status_h - input_h - 6;

    app->text_bottom = area_bottom;

    int rows = (area_bottom - area_top) / app->line_h;
    if (rows < 1)
        rows = 1;
    app->visible_rows = rows;
}

/* Верх области текста. Текст прижат к низу, как в терминале. */
static int text_area_y(const App *app)
{
    int rows_used = app->visible_rows;
    if ((int)app->view.row_count < rows_used)
        rows_used = (int)app->view.row_count;
    if (rows_used < 0)
        rows_used = 0;
    return app->text_bottom - rows_used * app->line_h;
}

static void copy_selection(App *app);

static void relayout(App *app)
{
    compute_layout(app);
    view_relayout(&app->view, app->font, app->renderer, app->theme,
                  app->width - 2 * APP_PADDING);
}

static void add_help(App *app)
{
    add_line(app, "Команды:", VIEW_HINT);
    add_line(app, "  /help            — эта справка", VIEW_HINT);
    add_line(app, "  /theme [имя]     — список или смена темы", VIEW_HINT);
    add_line(app, "  /color <роль> R G B — поменять цвет (bg, bot, user…)", VIEW_HINT);
    add_line(app, "  /theme save <имя>— сохранить текущие цвета в тему", VIEW_HINT);
    add_line(app, "  /say <тема>      — сочинить ответ цепью Маркова", VIEW_HINT);
    add_line(app, "  /stats           — статистика сессии", VIEW_HINT);
    add_line(app, "  /pet             — состояние бота", VIEW_HINT);
    add_line(app, "  /feed            — покормить бота", VIEW_HINT);
    add_line(app, "  /export [файл]   — сохранить диалог в Markdown", VIEW_HINT);
    add_line(app, "  /forget          — забыть выученные фразы", VIEW_HINT);
    add_line(app, "  /save, /load     — сохранить/загрузить память", VIEW_HINT);
    add_line(app, "  /clear           — очистить экран", VIEW_HINT);
    add_line(app, "  /about, /quit    — о программе и выход", VIEW_HINT);
    add_line(app, "Клавиши: PgUp/PgDn — прокрутка, ↑/↓ — история ввода, "
                  "мышь — выделить, Ctrl+C — копировать, Ctrl+V — вставка, "
                  "F11 — полный экран, Esc — выход.",
             VIEW_HINT);
}

static void save_state(App *app)
{
    if (app->cfg.memory_path[0] != '\0')
        chat_save(app->chat, app->cfg.memory_path);

    if (app->cfg.history_path[0] != '\0') {
        FILE *out = fopen(app->cfg.history_path, "wb");
        if (out != NULL) {
            for (size_t i = 0; i < input_history_count(&app->input); ++i) {
                const char *line = input_history_at(&app->input, i);
                if (line != NULL)
                    fprintf(out, "%s\n", line);
            }
            fclose(out);
        }
    }

    if (app->pet_path[0] != '\0')
        pet_save(&app->pet, app->pet_path);

    /* Геометрия окна: чтобы следующий запуск открылся там же и такого же
     * размера. Пишем только если окно реально существует. */
    if (app->config_path != NULL && app->cfg.save_geometry &&
        app->window != NULL && !app->screenshot) {
        SDL_GetWindowPosition(app->window, &app->cfg.window_x,
                              &app->cfg.window_y);
        SDL_GetWindowSize(app->window, &app->cfg.width, &app->cfg.height);
        config_save(&app->cfg, app->config_path);
    }
}

static void load_history(App *app)
{
    if (app->cfg.history_path[0] == '\0')
        return;

    FILE *in = fopen(app->cfg.history_path, "rb");
    if (in == NULL)
        return;

    char line[1024];
    while (fgets(line, (int)sizeof(line), in) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len > 0)
            input_remember(&app->input, line);
    }
    fclose(in);
}

static void schedule_reply(App *app, const char *text)
{
    free(app->pending_input);
    app->pending_input = utf8_dup(text);
    if (app->pending_input == NULL)
        return;

    size_t chars = utf8_strlen(text);
    uint32_t delay = APP_REPLY_BASE_MS + (uint32_t)chars * APP_REPLY_PER_CHAR_MS;
    if (delay > APP_REPLY_MAX_MS)
        delay = APP_REPLY_MAX_MS;
    app->pending_at = (uint32_t)SDL_GetTicks() + delay;
}

static void deliver_reply(App *app)
{
    if (app->pending_input == NULL) {
        app->pending_at = 0;
        return;
    }

    const char *reply = chat_reply(app->chat, app->pending_input);
    add_line(app, reply, VIEW_BOT);
    free(app->pending_input);
    app->pending_input = NULL;
    app->pending_at = 0;
    ++app->messages;
    app->view.scroll = 0;
}

static void run_command(App *app, const char *line)
{
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s", line);

    char *command = buffer;
    char *args = strchr(buffer, ' ');
    if (args != NULL) {
        *args++ = '\0';
        while (*args == ' ')
            ++args;
    } else {
        args = buffer + strlen(buffer);
    }

    if (strcmp(command, "help") == 0) {
        add_help(app);
    } else if (strcmp(command, "clear") == 0) {
        view_clear(&app->view);
        add_line(app, "Экран очищен. Память на месте.", VIEW_SYSTEM);
    } else if (strcmp(command, "stats") == 0) {
        uint32_t uptime = ((uint32_t)SDL_GetTicks() - app->start_ticks) / 1000u;
        addf(app, VIEW_SYSTEM,
             "Фраз в памяти: %zu (выучено: %zu) · сообщений: %llu · "
             "время: %02u:%02u · FPS: %d",
             chat_count(app->chat), chat_learned_count(app->chat),
             (unsigned long long)app->messages, uptime / 60u, uptime % 60u,
             (int)(app->fps + 0.5));
        addf(app, VIEW_HINT,
             "Цепь Маркова: %zu слов, %zu переходов · удивление: %.1f бит",
             chat_markov_vocab(app->chat), chat_markov_states(app->chat),
             chat_perplexity(app->chat, input_text(&app->input)));
    } else if (strcmp(command, "say") == 0) {
        if (*args == '\0') {
            add_line(app, "Напиши тему: /say котики", VIEW_WARNING);
        } else {
            add_line(app, chat_generate(app->chat, args, 1.0), VIEW_BOT);
            ++app->messages;
        }
    } else if (strcmp(command, "theme") == 0) {
        /* /theme save <имя> — сохранить текущие цвета как тему. */
        if (strncmp(args, "save", 4) == 0 && (args[4] == ' ' || args[4] == '\0')) {
            char *name = args + 4;
            while (*name == ' ')
                ++name;
            if (*name == '\0') {
                add_line(app, "Напиши имя: /theme save моя", VIEW_WARNING);
            } else {
                char path[CONFIG_PATH_MAX];
                snprintf(path, sizeof(path), "themes/%s.theme", name);
                snprintf(app->theme_storage.name,
                         sizeof(app->theme_storage.name), "%s", name);
                if (theme_save(&app->theme_storage, path))
                    addf(app, VIEW_SYSTEM, "Тема сохранена: %s", path);
                else
                    addf(app, VIEW_ERROR,
                         "Не удалось сохранить тему. Есть ли папка themes/?");
            }
        } else if (*args == '\0') {
            addf(app, VIEW_HINT, "Текущая тема: %s", app->theme->name);
            add_line(app, "Доступны: green, amber, ice, paper, а также "
                          "файлы themes/<имя>.theme", VIEW_HINT);
        } else {
            char path[CONFIG_PATH_MAX];
            snprintf(path, sizeof(path), "themes/%s.theme", args);

            const Theme *base = theme_by_name(args);
            if (base != NULL) {
                theme_copy(&app->theme_storage, base);
            } else if (theme_load(path, &app->theme_storage)) {
                snprintf(app->theme_storage.name,
                         sizeof(app->theme_storage.name), "%s", args);
            } else {
                addf(app, VIEW_WARNING, "Нет такой темы: %s", args);
                return;
            }

            app->theme = &app->theme_storage;
            snprintf(app->cfg.theme, sizeof(app->cfg.theme), "%s",
                     app->theme->name);
            relayout(app);
            addf(app, VIEW_SYSTEM, "Тема: %s", app->theme->name);
        }
    } else if (strcmp(command, "color") == 0) {
        char role[32] = {0};
        unsigned r = 0, g = 0, b = 0;
        if (sscanf(args, "%31s %u %u %u", role, &r, &g, &b) != 4) {
            add_line(app, "Формат: /color <роль> R G B, например /color bot 0 255 120",
                     VIEW_WARNING);
            add_line(app, "Роли: bg system user bot warning error hint input "
                          "status caret separator selection logo_a logo_b logo_c",
                     VIEW_HINT);
            return;
        }

        SDL_Color *color = theme_role(&app->theme_storage, role);
        if (color == NULL) {
            addf(app, VIEW_WARNING, "Нет такой роли: %s", role);
            return;
        }

        color->r = (Uint8)(r > 255 ? 255 : r);
        color->g = (Uint8)(g > 255 ? 255 : g);
        color->b = (Uint8)(b > 255 ? 255 : b);
        app->theme = &app->theme_storage;
        relayout(app);
        addf(app, VIEW_SYSTEM, "Цвет %s: %u %u %u", role, (unsigned)color->r,
             (unsigned)color->g, (unsigned)color->b);
    } else if (strcmp(command, "pet") == 0) {
        PetMood mood = pet_mood(&app->pet);
        addf(app, VIEW_SYSTEM, "Бот: %s · настроение %.0f · силы %.0f",
             pet_mood_name(mood), (double)app->pet.mood,
             (double)app->pet.energy);
        addf(app, VIEW_HINT, "Сообщений за всё время: %u",
             app->pet.messages);
    } else if (strcmp(command, "feed") == 0) {
        pet_feed(&app->pet, (uint32_t)SDL_GetTicks());
        add_line(app, "Бот поел. Доволен.", VIEW_SYSTEM);
    } else if (strcmp(command, "export") == 0) {
        const char *path = (*args != '\0') ? args : app->export_path;
        size_t count = view_line_count(&app->view);

        ExportLine *lines = (ExportLine *)calloc(count ? count : 1,
                                                 sizeof(ExportLine));
        if (lines == NULL) {
            add_line(app, "Не хватило памяти на экспорт.", VIEW_ERROR);
            return;
        }

        for (size_t i = 0; i < count; ++i) {
            const ViewLine *entry = view_line_at(&app->view, i);
            if (entry == NULL)
                continue;
            lines[i].text = entry->text;
            lines[i].kind = entry->kind;
            lines[i].at_ms = (entry->at_ms > app->start_ticks)
                                 ? entry->at_ms - app->start_ticks
                                 : 0;
        }

        char meta[256];
        snprintf(meta, sizeof(meta),
                 "Фраз в памяти: %zu · сообщений: %llu · тема: %s",
                 chat_count(app->chat), (unsigned long long)app->messages,
                 app->theme->name);

        bool ok = export_markdown(lines, count, path,
                                  "Codename Hello — диалог", meta);
        free(lines);

        if (ok)
            addf(app, VIEW_SYSTEM, "Диалог сохранён: %s", path);
        else
            addf(app, VIEW_ERROR, "Не удалось записать: %s", path);
    } else if (strcmp(command, "forget") == 0) {
        chat_forget(app->chat);
        add_line(app, "Выученное забыто. Встроенные фразы остались.", VIEW_SYSTEM);
    } else if (strcmp(command, "save") == 0) {
        if (chat_save(app->chat, app->cfg.memory_path))
            addf(app, VIEW_SYSTEM, "Память сохранена: %s", app->cfg.memory_path);
        else
            addf(app, VIEW_ERROR, "Не удалось сохранить: %s", app->cfg.memory_path);
    } else if (strcmp(command, "load") == 0) {
        size_t loaded = chat_load(app->chat, app->cfg.memory_path);
        addf(app, VIEW_SYSTEM, "Загружено фраз: %zu", loaded);
    } else if (strcmp(command, "about") == 0) {
        addf(app, VIEW_SYSTEM, "Codename Hello %s — консольный собеседник.", HELLO_VERSION);
        add_line(app, "2012–2013: SDL 1.2 · 2026: SDL2-порт, CMake, тесты.",
                 VIEW_HINT);
        add_line(app, "Исходники и исторические варианты: github.com/dpteam/Hello",
                 VIEW_HINT);
    } else if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        app->running = false;
    } else {
        addf(app, VIEW_WARNING, "Не знаю команду «%s». Попробуй /help.", command);
    }
}

static void submit(App *app)
{
    char *text = trim_dup(input_text(&app->input));
    if (text == NULL)
        return;

    if (text[0] == '\0') {
        add_line(app, "Пусто. Напиши что-нибудь.", VIEW_WARNING);
        free(text);
        return;
    }

    char *echo = (char *)malloc(strlen(text) + 4);
    if (echo != NULL) {
        sprintf(echo, "› %s", text);
        add_line(app, echo, VIEW_USER);
        free(echo);
    }

    input_remember(&app->input, text);
    input_clear(&app->input);
    ++app->messages;
    app->view.scroll = 0;

    if (text[0] == '/') {
        run_command(app, text + 1);
    } else {
        chat_learn(app->chat, text);
        pet_on_message(&app->pet, (uint32_t)SDL_GetTicks());
        schedule_reply(app, text);
    }

    free(text);
}

static void paste_clipboard(App *app)
{
    char *clip = SDL_GetClipboardText();
    if (clip == NULL)
        return;

    /* Вставляем только первую строку: многострочный ввод не нужен. */
    char *newline = strpbrk(clip, "\r\n");
    if (newline != NULL)
        *newline = '\0';

    if (clip[0] != '\0')
        input_insert(&app->input, clip);

    SDL_free(clip);
}

static void toggle_fullscreen(App *app)
{
    app->fullscreen = !app->fullscreen;
    if (!SDL_SetWindowFullscreen(
            app->window,
            app->fullscreen ? SDL_WINDOW_FULLSCREEN : 0)) {
        app->fullscreen = !app->fullscreen;
        return;
    }
    update_size(app);
    relayout(app);
}

static void handle_key(App *app, const SDL_KeyboardEvent *event)
{
    /* В SDL3 модификаторы и код клавиши лежат прямо в событии. */
    bool ctrl = (event->mod & SDL_KMOD_CTRL) != 0;
    bool shift = (event->mod & SDL_KMOD_SHIFT) != 0;

    switch (event->key) {
    case SDLK_ESCAPE:
        app->running = false;
        break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        submit(app);
        break;
    case SDLK_BACKSPACE:
        input_backspace(&app->input);
        break;
    case SDLK_DELETE:
        input_delete(&app->input);
        break;
    case SDLK_LEFT:
        if (ctrl) {
            input_home(&app->input);
        } else {
            input_move_left(&app->input);
        }
        break;
    case SDLK_RIGHT:
        if (ctrl) {
            input_end(&app->input);
        } else {
            input_move_right(&app->input);
        }
        break;
    case SDLK_HOME:
        input_home(&app->input);
        break;
    case SDLK_END:
        input_end(&app->input);
        break;
    case SDLK_UP:
        if (shift) {
            view_scroll(&app->view, 1, app->visible_rows);
        } else {
            input_history_prev(&app->input);
        }
        break;
    case SDLK_DOWN:
        if (shift) {
            view_scroll(&app->view, -1, app->visible_rows);
        } else {
            input_history_next(&app->input);
        }
        break;
    case SDLK_PAGEUP:
        view_scroll(&app->view, app->visible_rows - 1, app->visible_rows);
        break;
    case SDLK_PAGEDOWN:
        view_scroll(&app->view, -(app->visible_rows - 1), app->visible_rows);
        break;
    case SDLK_F1:
        add_help(app);
        break;
    case SDLK_F11:
        toggle_fullscreen(app);
        break;
    default:
        if (ctrl) {
            switch (event->key) {
            case SDLK_V:
                paste_clipboard(app);
                break;
            case SDLK_C:
                copy_selection(app);
                break;
            case SDLK_A:
                /* Выделить всё видимое. */
                if (app->view.row_count > 0) {
                    app->sel_active = true;
                    app->sel_anchor = 0;
                    app->sel_end = (int)app->view.row_count - 1;
                }
                break;
            case SDLK_L:
                view_clear(&app->view);
                break;
            case SDLK_S:
                if (chat_save(app->chat, app->cfg.memory_path))
                    addf(app, VIEW_SYSTEM, "Память сохранена: %s", app->cfg.memory_path);
                else
                    addf(app, VIEW_ERROR, "Не удалось сохранить память.");
                break;
            case SDLK_Q:
                app->running = false;
                break;
            default:
                break;
            }
        }
        break;
    }
}

static void copy_selection(App *app)
{
    if (!app->sel_active) {
        add_line(app, "Ничего не выделено. Выдели мышью.", VIEW_WARNING);
        return;
    }

    size_t cap = 64 * 1024;
    char *buffer = (char *)malloc(cap);
    if (buffer == NULL) {
        add_line(app, "Не хватило памяти на копирование.", VIEW_ERROR);
        return;
    }

    size_t len = view_rows_text(&app->view, app->sel_anchor, app->sel_end,
                                buffer, cap);
    if (len == 0) {
        free(buffer);
        return;
    }

    SDL_SetClipboardText(buffer);
    free(buffer);

    addf(app, VIEW_SYSTEM, "Скопировано: %zu символов.", len);
}

static void handle_mouse(App *app, const SDL_Event *event)
{
    int area_y = text_area_y(app);

    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event->button.button != SDL_BUTTON_LEFT)
            break;
        {
            int row = view_row_at(&app->view, (int)event->button.y, area_y,
                                  app->line_h, app->visible_rows);
            if (row < 0)
                break;
            app->sel_active = true;
            app->sel_anchor = row;
            app->sel_end = row;
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (!app->sel_active)
            break;
        if ((event->motion.state & SDL_BUTTON_LMASK) == 0)
            break;
        {
            int row = view_row_at(&app->view, (int)event->motion.y, area_y,
                                  app->line_h, app->visible_rows);
            if (row >= 0)
                app->sel_end = row;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event->button.button != SDL_BUTTON_LEFT)
            break;
        /* Одиночный клик без протяжки — снимаем выделение. */
        if (app->sel_anchor == app->sel_end) {
            int row = view_row_at(&app->view, (int)event->button.y, area_y,
                                  app->line_h, app->visible_rows);
            if (row >= 0 && row == app->sel_anchor)
                app->sel_active = false;
        }
        break;

    default:
        break;
    }
}
static void handle_event(App *app, const SDL_Event *event)
{
    switch (event->type) {
    case SDL_EVENT_QUIT:
        app->running = false;
        break;
    case SDL_EVENT_TEXT_INPUT:
        input_insert(&app->input, event->text.text);
        break;
    case SDL_EVENT_KEY_DOWN:
        handle_key(app, &event->key);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        /* В SDL3 прокрутка дробная — переводим в целые строки. */
        view_scroll(&app->view, (int)event->wheel.y * APP_SCROLL_STEP,
                    app->visible_rows);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_MOTION:
        handle_mouse(app, event);
        break;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        update_size(app);
        relayout(app);
        break;
    default:
        break;
    }
}

/* Самопроверка ввода.
 *
 * Нужна потому, что ошибка «SDL3 не присылает TEXT_INPUT» тихая:
 * программа запускается, Enter и стрелки работают, а буквы не
 * набираются. Ни компилятор, ни тесты ядра такое не поймают —
 * проблема на стыке с SDL.
 *
 * Проверяем три вещи: ввод включён на уровне SDL, символы попадают
 * в буфер, backspace удаляет символ UTF-8 целиком. */
static int run_selftest(App *app)
{
    int failures = 0;
    SDL_Event event;

    if (!SDL_TextInputActive(app->window)) {
        hello_log("ПРОВАЛ: текстовый ввод не активен "
                  "(SDL_StartTextInput не сработал)");
        ++failures;
    } else {
        hello_log("ок: текстовый ввод активен");
    }

    SDL_zero(event);
    event.type = SDL_EVENT_TEXT_INPUT;
    /* В SDL3 поле text — указатель, а не встроенный буфер. */
    event.text.text = "тест";
    handle_event(app, &event);

    if (strcmp(input_text(&app->input), "тест") != 0) {
        hello_log("ПРОВАЛ: в буфере «%s», ожидалось «тест»",
                  input_text(&app->input));
        ++failures;
    } else {
        hello_log("ок: символы попадают в буфер ввода");
    }

    SDL_zero(event);
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_BACKSPACE;
    handle_event(app, &event);

    if (strcmp(input_text(&app->input), "тес") != 0) {
        hello_log("ПРОВАЛ: после backspace «%s», ожидалось «тес»",
                  input_text(&app->input));
        ++failures;
    } else {
        hello_log("ок: backspace удаляет символ UTF-8 целиком");
    }

    /* Enter отправляет: строка уходит в диалог, буфер очищается. */
    SDL_zero(event);
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_RETURN;
    handle_event(app, &event);

    if (input_text(&app->input)[0] != '\0') {
        hello_log("ПРОВАЛ: после Enter буфер не очищен");
        ++failures;
    } else {
        hello_log("ок: Enter отправляет и очищает ввод");
    }

    hello_log(failures == 0 ? "САМОПРОВЕРКА ПРОЙДЕНА"
                            : "САМОПРОВЕРКА ПРОВАЛЕНА");
    return failures == 0 ? 0 : 1;
}

static void draw_status(App *app)
{
    char right[APP_STATUS_MAX];
    if (app->pending_input != NULL) {
        snprintf(right, sizeof(right), "печатает…");
    } else {
        snprintf(right, sizeof(right), "%s · %zu фраз · %d FPS",
                 pet_mood_name(pet_mood(&app->pet)), chat_count(app->chat),
                 (int)(app->fps + 0.5));
    }

    char left[APP_STATUS_MAX];
    if (app->sel_active) {
        int from = app->sel_anchor < app->sel_end ? app->sel_anchor : app->sel_end;
        int to = app->sel_anchor < app->sel_end ? app->sel_end : app->sel_anchor;
        snprintf(left, sizeof(left), "выделено строк: %d · Ctrl+C — копировать",
                 to - from + 1);
    } else {
        snprintf(left, sizeof(left), "Codename Hello %s", HELLO_VERSION);
    }

    int status_y = app->height - APP_PADDING - app->line_h;
    render_text_at(app->renderer, app->font, left, app->theme->status,
                   APP_PADDING, status_y);

    int w = 0;
    if (render_measure(app->font, right, &w, NULL))
        render_text_at(app->renderer, app->font, right, app->theme->status,
                       app->width - APP_PADDING - w, status_y);
}

/* SDL_Color (0..255) → SDL_FColor (0..1) с заданной прозрачностью.
 * В SDL3 вершины используют float-цвета. */
static SDL_FColor to_fcolor(SDL_Color c, float alpha)
{
    SDL_FColor out;
    out.r = (float)c.r / 255.0f;
    out.g = (float)c.g / 255.0f;
    out.b = (float)c.b / 255.0f;
    out.a = alpha;
    return out;
}

/* Треугольник из макетов 2015 года. Рисуется только в свободном месте
 * над текстом; если текста много — не рисуется вовсе. */
static void draw_logo(App *app, int area_y)
{
    if (app->visible_rows <= 0 || app->line_h <= 0)
        return;

    /* Свободное место: от верхнего отступа до первой строки текста. */
    int top = APP_PADDING;
    int bottom = area_y;
    int free_h = bottom - top;
    if (free_h < app->line_h * 4)
        return;

    float cx = (float)app->width * 0.5f;
    float cy = (float)(top + bottom) * 0.5f;

    float size = (float)free_h * 0.62f;
    float max_w = (float)app->width * 0.28f;
    if (size > max_w)
        size = max_w;
    if (size < 40.0f)
        return;

    SDL_Vertex verts[3];

    verts[0].position.x = cx;
    verts[0].position.y = cy - size * 0.5f;
    verts[0].color = to_fcolor(app->theme->logo_a, 0.22f);
    verts[0].tex_coord.x = 0.0f;
    verts[0].tex_coord.y = 0.0f;

    verts[1].position.x = cx - size * 0.46f;
    verts[1].position.y = cy + size * 0.5f;
    verts[1].color = to_fcolor(app->theme->logo_b, 0.22f);
    verts[1].tex_coord.x = 0.0f;
    verts[1].tex_coord.y = 0.0f;

    verts[2].position.x = cx + size * 0.46f;
    verts[2].position.y = cy + size * 0.5f;
    verts[2].color = to_fcolor(app->theme->logo_c, 0.22f);
    verts[2].tex_coord.x = 0.0f;
    verts[2].tex_coord.y = 0.0f;

    const int indices[3] = {0, 1, 2};
    SDL_RenderGeometry(app->renderer, NULL, verts, 3, indices, 3);
}

static void draw_selection(App *app, int area_y)
{
    if (!app->sel_active || app->line_h <= 0)
        return;

    int from = app->sel_anchor < app->sel_end ? app->sel_anchor : app->sel_end;
    int to = app->sel_anchor < app->sel_end ? app->sel_end : app->sel_anchor;

    int start = view_visible_start(&app->view, app->visible_rows);

    SDL_SetRenderDrawColor(app->renderer, app->theme->selection.r,
                           app->theme->selection.g, app->theme->selection.b, 120);

    for (int row = from; row <= to; ++row) {
        int slot = row - start;
        if (slot < 0 || slot >= app->visible_rows)
            continue;

        SDL_FRect rect = {(float)APP_PADDING,
                          (float)(area_y + slot * app->line_h),
                          (float)(app->width - 2 * APP_PADDING),
                          (float)app->line_h};
        SDL_RenderFillRect(app->renderer, &rect);
    }
}

static void draw_frame(App *app, bool show_caret)
{
    SDL_SetRenderDrawColor(app->renderer, app->theme->bg.r, app->theme->bg.g,
                           app->theme->bg.b, app->theme->bg.a);
    SDL_RenderClear(app->renderer);

    int status_h = app->line_h;
    int input_h = app->line_h;
    int status_y = app->height - APP_PADDING - status_h;
    int input_y = status_y - input_h;
    int area_bottom = input_y - 6;

    int area_y = text_area_y(app);

    draw_logo(app, area_y);
    draw_selection(app, area_y);

    view_draw(&app->view, app->renderer, APP_PADDING, area_y, app->line_h,
              app->visible_rows);

    /* Индикаторы прокрутки. */
    int max_scroll = view_max_scroll(&app->view, app->visible_rows);
    int scroll = view_scroll_offset(&app->view);
    if (scroll < max_scroll)
        render_text_at(app->renderer, app->font, "↑", app->theme->hint,
                       app->width - APP_PADDING - 8, APP_PADDING);
    if (scroll > 0)
        render_text_at(app->renderer, app->font, "↓", app->theme->hint,
                       app->width - APP_PADDING - 8,
                       area_bottom - app->line_h);

    /* Разделитель над строкой ввода. */
    SDL_SetRenderDrawColor(app->renderer, app->theme->separator.r,
                           app->theme->separator.g, app->theme->separator.b,
                           app->theme->separator.a);
    SDL_RenderLine(app->renderer, (float)APP_PADDING, (float)(input_y - 3),
                   (float)(app->width - APP_PADDING), (float)(input_y - 3));

    /* Строка ввода с курсором. */
    char line[INPUT_MAX_BYTES + 8];
    snprintf(line, sizeof(line), "> %s", input_text(&app->input));
    size_t caret = 2 + input_cursor(&app->input);
    render_input(app->renderer, app->font, line, caret, app->theme->input,
                 app->theme->caret, APP_PADDING, input_y, show_caret);

    draw_status(app);
    SDL_RenderPresent(app->renderer);
}

static void add_intro(App *app)
{
    addf(app, VIEW_SYSTEM, "Codename Hello %s — консольный собеседник.", HELLO_VERSION);
    addf(app, VIEW_BOT, "Здарова, %s ёпта", chat_random(app->chat));
    add_line(app, "Esc — выход · PgUp/PgDn — история · F1 — помощь · /help — команды",
             VIEW_HINT);
}

static int save_screenshot(App *app, const char *path)
{
    /* В SDL3 поверхность создаёт сама SDL_RenderReadPixels. */
    SDL_Surface *shot = SDL_RenderReadPixels(app->renderer, NULL);
    if (shot == NULL) {
        hello_log("Не удалось прочитать пиксели: %s", SDL_GetError());
        return 1;
    }

    int rc = SDL_SaveBMP(shot, path) ? 0 : 1;
    if (rc != 0)
        hello_log("Не удалось сохранить %s: %s", path, SDL_GetError());

    SDL_DestroySurface(shot);
    return rc;
}

static void demo_conversation(App *app)
{
    add_line(app, "Codename Hello " HELLO_VERSION " — консольный собеседник",
             VIEW_SYSTEM);
    add_line(app, "Здарова, Привет ёпта", VIEW_BOT);
    add_line(app, "› привет. ты кто?", VIEW_USER);
    add_line(app, "Я думаю...", VIEW_BOT);
    add_line(app, "› что ты умеешь?", VIEW_USER);
    add_line(app, "Может быть", VIEW_BOT);
    addf(app, VIEW_USER, "› /theme %s", app->theme->name);
    addf(app, VIEW_SYSTEM, "Тема: %s", app->theme->name);
    add_line(app, "Esc — выход · PgUp/PgDn — история · F1 — помощь · /help — команды",
             VIEW_HINT);
}
int app_run(const AppOptions *options)
{
    App app;
    int exit_code = 0;
    memset(&app, 0, sizeof(app));
    app.cfg = options->config;
    app.config_path = options->config_path;
    /* Самопроверке окно не нужно показывать: она проверяет ввод. */
    app.screenshot = options->screenshot_path != NULL || options->selftest;
    app.running = true;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        hello_log("SDL_Init: %s", SDL_GetError());
        return 1;
    }

    if (!TTF_Init()) {
        hello_log("TTF_Init: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    char font_path[CONFIG_PATH_MAX * 2];
    if (!hello_find_font(&app.cfg, font_path, sizeof(font_path))) {
        hello_show_error(APP_WINDOW_TITLE,
                         "Не найден шрифт DejaVu Sans Mono Bold.\n"
                         "Положи его в assets/fonts/ или укажи путь "
                         "в hello.cfg (ключ font).");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    if (!hello_open_window(&app.cfg, app.screenshot, APP_WINDOW_TITLE,
                           &app.window, &app.renderer)) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    app.font = TTF_OpenFont(font_path, (float)app.cfg.font_size);
    if (app.font == NULL) {
        hello_log("TTF_OpenFont(%s): %s", font_path, SDL_GetError());
        hello_close_window(app.window, app.renderer);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);

    app.theme = theme_by_name(app.cfg.theme);
    if (app.theme == NULL)
        app.theme = theme_default();

    /* Своя копия: /color меняет именно её, встроенные темы остаются
     * нетронутыми. */
    theme_copy(&app.theme_storage, app.theme);
    app.theme = &app.theme_storage;
    snprintf(app.cfg.theme, sizeof(app.cfg.theme), "%s", app.theme->name);

    app.chat = chat_create((unsigned)SDL_GetTicks());
    if (app.chat == NULL) {
        hello_log("Не хватило памяти на память бота.");
        TTF_CloseFont(app.font);
        hello_close_window(app.window, app.renderer);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    view_init(&app.view, (size_t)app.cfg.max_lines);
    input_init(&app.input);

    /* Состояние «тамагочи» и путь экспорта — рядом с памятью. */
    config_profile_path(&app.cfg, "hello.pet", app.pet_path,
                        sizeof(app.pet_path));
    config_profile_path(&app.cfg, "hello.export.md", app.export_path,
                        sizeof(app.export_path));

    pet_init(&app.pet);
    if (pet_load(&app.pet, app.pet_path))
        app.pet.last_update_ms = 0; /* отсчёт начнём с этого запуска */

    update_size(&app);
    compute_layout(&app);

    if (options->selftest) {
        exit_code = run_selftest(&app);
        app.running = false;
        goto cleanup;
    }

    if (app.screenshot) {
        demo_conversation(&app);
        draw_frame(&app, true);
        exit_code = save_screenshot(&app, options->screenshot_path);
        app.running = false;
        goto cleanup;
    }

    add_intro(&app);

    if (app.cfg.memory_path[0] != '\0') {
        size_t loaded = chat_load(app.chat, app.cfg.memory_path);
        if (loaded > 0)
            addf(&app, VIEW_HINT, "Память загружена: %zu фраз.", loaded);
    }
    load_history(&app);

    app.start_ticks = (uint32_t)SDL_GetTicks();
    app.last_frame_ticks = app.start_ticks;

    int target_frame_ms = 16;
    if (!app.cfg.vsync && app.cfg.fps_limit > 0)
        target_frame_ms = 1000 / app.cfg.fps_limit;

    while (app.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            handle_event(&app, &event);

        uint32_t now = (uint32_t)SDL_GetTicks();
        pet_update(&app.pet, now);

        if (app.pending_at != 0 && now >= app.pending_at)
            deliver_reply(&app);

        bool caret = ((now / APP_CARET_BLINK_MS) % 2) == 0;
        draw_frame(&app, caret);
        ++app.frames;

        uint32_t frame_time = now - app.last_frame_ticks;
        app.last_frame_ticks = now;
        if (frame_time > 0) {
            double instant = 1000.0 / (double)frame_time;
            app.fps = app.fps == 0.0 ? instant : app.fps * 0.9 + instant * 0.1;
        }

        if (!app.cfg.vsync) {
            uint32_t elapsed = (uint32_t)SDL_GetTicks() - now;
            if (elapsed < (uint32_t)target_frame_ms)
                SDL_Delay((uint32_t)target_frame_ms - elapsed);
        }
    }

    save_state(&app);

cleanup:
    input_free(&app.input);
    view_destroy(&app.view);
    chat_destroy(app.chat);
    TTF_CloseFont(app.font);
    hello_close_window(app.window, app.renderer);
    TTF_Quit();
    SDL_Quit();
    return exit_code;
}
