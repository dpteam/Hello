/*
 * render.h — тонкая обёртка над SDL_ttf: текст → текстура.
 */
#ifndef HELLO_RENDER_H
#define HELLO_RENDER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

/* NULL при ошибке (поверхность/текстура/шрифт). */
SDL_Texture *render_text(SDL_Renderer *ren, TTF_Font *font,
                         const char *utf8, SDL_Color color);

/* Рисует текст в точке (x, y). Возвращает ширину или -1. */
int render_text_at(SDL_Renderer *ren, TTF_Font *font, const char *utf8,
                   SDL_Color color, int x, int y);

/* Ширина/высота строки в пикселях. */
bool render_measure(TTF_Font *font, const char *utf8, int *w, int *h);

/* Строка ввода: текст + мигающий курсор.
 * cursor_byte — смещение в байтах (граница UTF-8). */
void render_input(SDL_Renderer *ren, TTF_Font *font, const char *utf8,
                  size_t cursor_byte, SDL_Color fg, SDL_Color caret,
                  int x, int y, bool show_caret);

#endif /* HELLO_RENDER_H */
