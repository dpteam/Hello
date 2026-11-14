/*
 * theme.h — цветовые схемы интерфейса.
 *
 * Встроенные темы неизменяемы. Пользователь может скопировать тему
 * в свою (theme_copy) и менять цвета в рантайме, а также сохранить
 * результат в файл.
 */
#ifndef HELLO_THEME_H
#define HELLO_THEME_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define THEME_ROLE_MAX 24

typedef struct Theme {
    char name[THEME_ROLE_MAX];
    SDL_Color bg;
    SDL_Color system;
    SDL_Color user;
    SDL_Color bot;
    SDL_Color warning;
    SDL_Color error;
    SDL_Color hint;
    SDL_Color input;
    SDL_Color status;
    SDL_Color caret;
    SDL_Color separator;
    SDL_Color selection;
    SDL_Color logo_a;
    SDL_Color logo_b;
    SDL_Color logo_c;
} Theme;

int theme_count(void);
const Theme *theme_at(int index);
const Theme *theme_by_name(const char *name);
const Theme *theme_default(void);

/* Копирует тему. dst и src могут указывать на одно и то же. */
void theme_copy(Theme *dst, const Theme *src);

/* Находит роль по имени: "bg", "system", "user", "bot", "warning",
 * "error", "hint", "input", "status", "caret", "separator",
 * "selection", "logo_a", "logo_b", "logo_c".
 * Возвращает указатель на поле или NULL. */
SDL_Color *theme_role(Theme *theme, const char *role);

/* Имена всех ролей — для подсказки в /color. */
size_t theme_role_count(void);
const char *theme_role_name(size_t index);

/* Читает и пишет тему в текстовый файл (key = r g b). */
bool theme_save(const Theme *theme, const char *path);
bool theme_load(const char *path, Theme *out);

#endif /* HELLO_THEME_H */
