/*
 * theme.h — цветовые схемы интерфейса.
 */
#ifndef HELLO_THEME_H
#define HELLO_THEME_H

#include <SDL.h>

typedef struct Theme {
    const char *name;
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
} Theme;

int theme_count(void);
const Theme *theme_at(int index);
const Theme *theme_by_name(const char *name);
const Theme *theme_default(void);

#endif /* HELLO_THEME_H */
