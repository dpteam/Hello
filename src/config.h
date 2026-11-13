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
#define CONFIG_PROFILE_MAX 48

/* Значение window_x / window_y: «пусть менеджер окон решит». */
#define CONFIG_WINDOW_POS_ANY (-32768)

typedef struct {
    int width;
    int height;

    /* Позиция окна. CONFIG_WINDOW_POS_ANY — центрировать. */
    int window_x;
    int window_y;
    bool save_geometry;

    int font_size;
    int max_lines;
    int fps_limit; /* 0 — только vsync */
    bool vsync;

    char theme[CONFIG_THEME_MAX];
    char font_path[CONFIG_PATH_MAX];
    char memory_path[CONFIG_PATH_MAX];
    char history_path[CONFIG_PATH_MAX];
    char plugins_path[CONFIG_PATH_MAX];
    char profile[CONFIG_PROFILE_MAX];
} Config;

void config_defaults(Config *cfg);

/* false, если файл не открылся. Отдельные кривые строки
 * игнорируются: конфиг не должен ронять приложение. */
bool config_load(Config *cfg, const char *path);

/* Атомарная запись через временный файл. */
bool config_save(const Config *cfg, const char *path);

/* Путь к файлу памяти с учётом профиля: «hello.memory» превращается
 * в «hello.work.memory» для профиля «work». Пустой профиль — как есть. */
void config_profile_path(const Config *cfg, const char *base, char *out,
                         size_t cap);

#endif /* HELLO_CONFIG_H */
