/*
 * config.h — простой ini-подобный конфиг (key = value).
 * Без зависимостей, чтобы читаться тестами.
 */
#ifndef HELLO_CONFIG_H
#define HELLO_CONFIG_H

#include <stdbool.h>
#include <stddef.h>

#define CONFIG_PATH_MAX 512
#define CONFIG_THEME_MAX 32

typedef struct {
    int width;
    int height;
    int font_size;
    int max_lines;
    int fps_limit; /* 0 — только vsync */
    bool vsync;

    char theme[CONFIG_THEME_MAX];
    char font_path[CONFIG_PATH_MAX];
    char memory_path[CONFIG_PATH_MAX];
    char history_path[CONFIG_PATH_MAX];
} Config;

void config_defaults(Config *cfg);

/* false, если файл не открылся. Отдельные кривые строки
 * игнорируются: конфиг не должен ронять приложение. */
bool config_load(Config *cfg, const char *path);

/* Атомарная запись через временный файл. */
bool config_save(const Config *cfg, const char *path);

#endif /* HELLO_CONFIG_H */
