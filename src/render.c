#include "render.h"

#include <stdlib.h>
#include <string.h>

#define RENDER_PREFIX_MAX 2048

SDL_Texture *render_text(SDL_Renderer *ren, TTF_Font *font,
                         const char *utf8, SDL_Color color)
{
    if (ren == NULL || font == NULL || utf8 == NULL || utf8[0] == '\0')
        return NULL;

    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, utf8, color);
    if (surface == NULL)
        return NULL;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool render_measure(TTF_Font *font, const char *utf8, int *w, int *h)
{
    int width = 0, height = 0;
    if (font == NULL || utf8 == NULL)
        return false;
    if (TTF_SizeUTF8(font, utf8, &width, &height) != 0)
        return false;
    if (w)
        *w = width;
    if (h)
        *h = height;
    return true;
}

int render_text_at(SDL_Renderer *ren, TTF_Font *font, const char *utf8,
                   SDL_Color color, int x, int y)
{
    SDL_Texture *texture = render_text(ren, font, utf8, color);
    if (texture == NULL)
        return -1;

    int w = 0, h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(ren, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    return w;
}

void render_input(SDL_Renderer *ren, TTF_Font *font, const char *utf8,
                  size_t cursor_byte, SDL_Color fg, SDL_Color caret,
                  int x, int y, bool show_caret)
{
    if (ren == NULL || font == NULL || utf8 == NULL)
        return;

    int line_h = TTF_FontLineSkip(font);
    int width = 0;
    int height = 0;

    if (utf8[0] != '\0' && render_measure(font, utf8, &width, &height)) {
        SDL_Texture *texture = render_text(ren, font, utf8, fg);
        if (texture != NULL) {
            SDL_Rect dst = {x, y, width, height};
            SDL_RenderCopy(ren, texture, NULL, &dst);
            SDL_DestroyTexture(texture);
        }
    }

    if (!show_caret)
        return;

    /* Ширина префикса до курсора. */
    char prefix[RENDER_PREFIX_MAX];
    size_t len = cursor_byte;
    if (len > strlen(utf8))
        len = strlen(utf8);
    if (len >= sizeof(prefix))
        len = sizeof(prefix) - 1;
    memcpy(prefix, utf8, len);
    prefix[len] = '\0';

    int caret_x = x;
    if (len > 0 && render_measure(font, prefix, &width, NULL))
        caret_x = x + width;

    SDL_Rect bar = {caret_x, y + 1, 2, line_h - 2};
    SDL_SetRenderDrawColor(ren, caret.r, caret.g, caret.b, caret.a);
    SDL_RenderFillRect(ren, &bar);
}
