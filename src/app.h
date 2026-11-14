#ifndef HELLO_APP_H
#define HELLO_APP_H

#include <stdbool.h>

#include "config.h"

typedef struct {
    Config config;

    /* Путь к конфигу: при выходе туда сохраняется геометрия окна.
     * NULL — не сохранять. */
    const char *config_path;

    /* NULL — обычный запуск; иначе отрисовать демо-кадр в BMP и выйти. */
    const char *screenshot_path;

    /* Проверить ввод и выйти с кодом 0/1, не открывая интерактивного окна. */
    bool selftest;
} AppOptions;

/* 0 — успех. */
int app_run(const AppOptions *options);

#endif /* HELLO_APP_H */
