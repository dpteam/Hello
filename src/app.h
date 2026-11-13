#ifndef HELLO_APP_H
#define HELLO_APP_H

#include "config.h"

typedef struct {
    Config config;
    const char *screenshot_path; /* NULL — обычный запуск */
} AppOptions;

/* 0 — успех. */
int app_run(const AppOptions *options);

#endif /* HELLO_APP_H */
