/*
 * main.c — разбор аргументов и запуск.
 *
 * Codename Hello: изначально (2012–2013) — SDL 1.2 чат-бот
 * с зелёным моноширинным текстом. В 2026-м переехал на SDL2,
 * но идея та же: память фраз + нечёткое совпадение.
 */
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "config.h"
#include "theme.h"
#include "version.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void attach_console(void)
{
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *out = freopen("CONOUT$", "w", stdout);
        FILE *err = freopen("CONOUT$", "w", stderr);
        (void)out;
        (void)err;
    }
#endif
}

static void print_usage(void)
{
    puts("Codename Hello " HELLO_VERSION " — консольный собеседник");
    puts("");
    puts("Использование: hello [параметры]");
    puts("");
    puts("Параметры:");
    puts("  --config <файл>      прочитать настройки (по умолчанию hello.cfg)");
    puts("  --width <пикс>       ширина окна");
    puts("  --height <пикс>      высота окна");
    puts("  --font <файл>        путь к TTF-шрифту");
    puts("  --font-size <пикс>   размер шрифта");
    puts("  --theme <имя>        green | amber | ice | paper");
    puts("  --memory <файл>      файл памяти бота");
    puts("  --history <файл>     файл истории ввода");
    puts("  --no-vsync           отключить вертикальную синхронизацию");
    puts("  --screenshot <файл>  отрисовать демо-кадр в BMP и выйти");
    puts("  -h, --help           эта справка");
    puts("  -V, --version        версия");
    puts("");
    puts("Клавиши: Esc — выход, PgUp/PgDn — прокрутка, F1 — помощь.");
}

static bool parse_int(const char *text, int *out)
{
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0')
        return false;
    *out = (int)value;
    return true;
}

static void copy_arg(char *dst, size_t cap, const char *src)
{
    if (cap == 0)
        return;
    size_t len = strlen(src);
    if (len >= cap)
        len = cap - 1;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

int main(int argc, char *argv[])
{
    attach_console();

    Config cfg;
    config_defaults(&cfg);

    const char *config_path = NULL;
    const char *screenshot = NULL;
    bool have_explicit_config = false;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            print_usage();
            return 0;
        }
        if (strcmp(arg, "-V") == 0 || strcmp(arg, "--version") == 0) {
            printf("Codename Hello %s\n", HELLO_VERSION);
            return 0;
        }

        const char *value = (i + 1 < argc) ? argv[i + 1] : NULL;

        if (strcmp(arg, "--config") == 0 && value != NULL) {
            config_path = value;
            have_explicit_config = true;
            ++i;
        } else if (strcmp(arg, "--width") == 0 && value != NULL) {
            parse_int(value, &cfg.width);
            ++i;
        } else if (strcmp(arg, "--height") == 0 && value != NULL) {
            parse_int(value, &cfg.height);
            ++i;
        } else if (strcmp(arg, "--font") == 0 && value != NULL) {
            copy_arg(cfg.font_path, sizeof(cfg.font_path), value);
            ++i;
        } else if (strcmp(arg, "--font-size") == 0 && value != NULL) {
            parse_int(value, &cfg.font_size);
            ++i;
        } else if (strcmp(arg, "--theme") == 0 && value != NULL) {
            copy_arg(cfg.theme, sizeof(cfg.theme), value);
            ++i;
        } else if (strcmp(arg, "--memory") == 0 && value != NULL) {
            copy_arg(cfg.memory_path, sizeof(cfg.memory_path), value);
            ++i;
        } else if (strcmp(arg, "--history") == 0 && value != NULL) {
            copy_arg(cfg.history_path, sizeof(cfg.history_path), value);
            ++i;
        } else if (strcmp(arg, "--no-vsync") == 0) {
            cfg.vsync = false;
        } else if (strcmp(arg, "--screenshot") == 0 && value != NULL) {
            screenshot = value;
            ++i;
        } else {
            fprintf(stderr, "Неизвестный параметр: %s\n", arg);
            fprintf(stderr, "Справка: hello --help\n");
            return 2;
        }
    }

    if (config_path == NULL)
        config_path = "hello.cfg";

    if (!config_load(&cfg, config_path) && have_explicit_config) {
        fprintf(stderr, "Не удалось прочитать конфиг: %s\n", config_path);
        return 2;
    }

    /* Валидация после конфига и аргументов. */
    if (cfg.width < 320)
        cfg.width = 320;
    if (cfg.height < 240)
        cfg.height = 240;
    if (cfg.font_size < 6)
        cfg.font_size = 6;
    if (cfg.font_size > 96)
        cfg.font_size = 96;

    if (theme_by_name(cfg.theme) == NULL)
        copy_arg(cfg.theme, sizeof(cfg.theme), theme_default()->name);

    AppOptions options;
    options.config = cfg;
    options.screenshot_path = screenshot;
    return app_run(&options);
}
