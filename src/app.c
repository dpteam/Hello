#include "app.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include "input.h"
#include "platform.h"
#include "render.h"
#include "theme.h"
#include "utf8.h"
#include "version.h"
#include "view.h"

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
    const Theme *theme;

    Chat *chat;
    View view;
    Input input;

    TTF_Font *font;
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width;
    int height;
    int line_h;
    int visible_rows;

    bool running;
    bool screenshot;
    bool fullscreen;

    uint32_t pending_at; /* 0 — ответа в очереди нет */
    char *pending_input;

    uint64_t messages;
    uint64_t frames;
    uint32_t start_ticks;
    uint32_t last_frame_ticks;
    double fps;

    const char *config_path;
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
    if (SDL_GetRendererOutputSize(app->renderer, &w, &h) != 0 || w <= 0 || h <= 0)
        SDL_GetWindowSize(app->window, &w, &h);
    app->width = w;
    app->height = h;
}

static void compute_layout(App *app)
{
    app->line_h = TTF_FontLineSkip(app->font);

    int status_h = app->line_h;
    int input_h = app->line_h;
    int area_top = APP_PADDING;
    int area_bottom = app->height - APP_PADDING - status_h - input_h - 6;

    int rows = (area_bottom - area_top) / app->line_h;
    if (rows < 1)
        rows = 1;
    app->visible_rows = rows;
}

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
    add_line(app, "  /say <тема>      — сочинить ответ цепью Маркова", VIEW_HINT);
    add_line(app, "  /stats           — статистика сессии", VIEW_HINT);
    add_line(app, "  /forget          — забыть выученные фразы", VIEW_HINT);
    add_line(app, "  /save, /load     — сохранить/загрузить память", VIEW_HINT);
    add_line(app, "  /clear           — очистить экран", VIEW_HINT);
    add_line(app, "  /about, /quit    — о программе и выход", VIEW_HINT);
    add_line(app, "Клавиши: PgUp/PgDn — прокрутка, ↑/↓ — история ввода, "
                  "Ctrl+V — вставка, F11 — полный экран, Esc — выход.",
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
    app->pending_at = SDL_GetTicks() + delay;
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
        uint32_t uptime = (SDL_GetTicks() - app->start_ticks) / 1000u;
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
        if (*args == '\0') {
            addf(app, VIEW_HINT, "Текущая тема: %s. Доступны: green, amber, ice, paper.",
                 app->theme->name);
        } else {
            const Theme *theme = theme_by_name(args);
            if (theme == NULL) {
                addf(app, VIEW_WARNING, "Нет такой темы: %s", args);
            } else {
                app->theme = theme;
                snprintf(app->cfg.theme, sizeof(app->cfg.theme), "%s", theme->name);
                relayout(app);
                addf(app, VIEW_SYSTEM, "Тема: %s", theme->name);
            }
        }
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
    if (SDL_SetWindowFullscreen(app->window,
                                app->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0) {
        app->fullscreen = !app->fullscreen;
        return;
    }
    update_size(app);
    relayout(app);
}

static void handle_key(App *app, const SDL_KeyboardEvent *event)
{
    bool ctrl = (event->keysym.mod & KMOD_CTRL) != 0;
    bool shift = (event->keysym.mod & KMOD_SHIFT) != 0;

    switch (event->keysym.sym) {
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
            switch (event->keysym.sym) {
            case SDLK_v:
                paste_clipboard(app);
                break;
            case SDLK_l:
                view_clear(&app->view);
                break;
            case SDLK_s:
                if (chat_save(app->chat, app->cfg.memory_path))
                    addf(app, VIEW_SYSTEM, "Память сохранена: %s", app->cfg.memory_path);
                else
                    addf(app, VIEW_ERROR, "Не удалось сохранить память.");
                break;
            case SDLK_q:
                app->running = false;
                break;
            default:
                break;
            }
        }
        break;
    }
}

static void handle_event(App *app, const SDL_Event *event)
{
    switch (event->type) {
    case SDL_QUIT:
        app->running = false;
        break;
    case SDL_TEXTINPUT:
        input_insert(&app->input, event->text.text);
        break;
    case SDL_KEYDOWN:
        handle_key(app, &event->key);
        break;
    case SDL_MOUSEWHEEL:
        view_scroll(&app->view, event->wheel.y * APP_SCROLL_STEP, app->visible_rows);
        break;
    case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
            event->window.event == SDL_WINDOWEVENT_RESIZED) {
            update_size(app);
            relayout(app);
        }
        break;
    default:
        break;
    }
}

static void draw_status(App *app)
{
    char right[APP_STATUS_MAX];
    if (app->pending_input != NULL) {
        snprintf(right, sizeof(right), "печатает…");
    } else {
        snprintf(right, sizeof(right), "%zu фраз · %d FPS", chat_count(app->chat),
                 (int)(app->fps + 0.5));
    }

    char left[APP_STATUS_MAX];
    snprintf(left, sizeof(left), "Codename Hello %s", HELLO_VERSION);

    int status_y = app->height - APP_PADDING - app->line_h;
    render_text_at(app->renderer, app->font, left, app->theme->status,
                   APP_PADDING, status_y);

    int w = 0;
    if (render_measure(app->font, right, &w, NULL))
        render_text_at(app->renderer, app->font, right, app->theme->status,
                       app->width - APP_PADDING - w, status_y);
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

    /* Как в терминале: текст прижат к нижнему краю, пустое место
     * остаётся сверху. */
    int rows_used = app->visible_rows;
    if ((int)app->view.row_count < rows_used)
        rows_used = (int)app->view.row_count;
    if (rows_used < 0)
        rows_used = 0;

    int area_y = area_bottom - rows_used * app->line_h;

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
    SDL_RenderDrawLine(app->renderer, APP_PADDING, input_y - 3,
                       app->width - APP_PADDING, input_y - 3);

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
    SDL_Surface *shot = SDL_CreateRGBSurfaceWithFormat(
        0, app->width, app->height, 32, SDL_PIXELFORMAT_ARGB8888);
    if (shot == NULL) {
        hello_log("Не удалось создать поверхность для скриншота: %s", SDL_GetError());
        return 1;
    }

    if (SDL_RenderReadPixels(app->renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                             shot->pixels, shot->pitch) != 0) {
        hello_log("Не удалось прочитать пиксели: %s", SDL_GetError());
        SDL_FreeSurface(shot);
        return 1;
    }

    int rc = SDL_SaveBMP(shot, path) == 0 ? 0 : 1;
    if (rc != 0)
        hello_log("Не удалось сохранить %s: %s", path, SDL_GetError());
    SDL_FreeSurface(shot);
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
    app.screenshot = options->screenshot_path != NULL;
    app.running = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        hello_log("SDL_Init: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        hello_log("TTF_Init: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    char font_path[CONFIG_PATH_MAX * 2];
    if (!hello_find_font(&app.cfg, font_path, sizeof(font_path))) {
        hello_log("Шрифт не найден. Положи DejaVuSansMono-Bold.ttf в assets/fonts/ "
                  "или укажи путь в hello.cfg (ключ font).");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, APP_WINDOW_TITLE,
                                 "Не найден шрифт DejaVu Sans Mono Bold.\n"
                                 "Укажи путь в hello.cfg (ключ font).",
                                 NULL);
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

    app.font = TTF_OpenFont(font_path, app.cfg.font_size);
    if (app.font == NULL) {
        hello_log("TTF_OpenFont(%s): %s", font_path, TTF_GetError());
        hello_close_window(app.window, app.renderer);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);

    app.theme = theme_by_name(app.cfg.theme);
    if (app.theme == NULL) {
        app.theme = theme_default();
        snprintf(app.cfg.theme, sizeof(app.cfg.theme), "%s", app.theme->name);
    }

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

    update_size(&app);
    compute_layout(&app);

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

    app.start_ticks = SDL_GetTicks();
    app.last_frame_ticks = app.start_ticks;

    int target_frame_ms = 16;
    if (!app.cfg.vsync && app.cfg.fps_limit > 0)
        target_frame_ms = 1000 / app.cfg.fps_limit;

    while (app.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            handle_event(&app, &event);

        uint32_t now = SDL_GetTicks();
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
            uint32_t elapsed = SDL_GetTicks() - now;
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
