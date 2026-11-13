#include "platform.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "icon_rgba.h"

#ifdef _WIN32
#include <windows.h>
#endif

void hello_attach_console(void)
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

void hello_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    fflush(stderr);
}

const char *hello_base_path(void)
{
    static char path[CONFIG_PATH_MAX];
    static bool resolved = false;

    if (!resolved) {
        resolved = true;
        char *dir = SDL_GetBasePath();
        if (dir != NULL) {
            size_t len = strlen(dir);
            if (len >= sizeof(path))
                len = sizeof(path) - 1;
            memcpy(path, dir, len);
            path[len] = '\0';
            SDL_free(dir);
        } else {
            path[0] = '\0';
        }
    }
    return path;
}

bool hello_file_exists(const char *path)
{
    if (path == NULL || path[0] == '\0')
        return false;
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return false;
    fclose(f);
    return true;
}

static bool is_absolute(const char *path)
{
    if (path == NULL || path[0] == '\0')
        return false;
    if (path[0] == '/')
        return true;
    if (path[0] != '\0' && path[1] == ':')
        return true; /* C:\... */
    return false;
}

void hello_resolve_path(const char *relative, char *out, size_t cap)
{
    if (out == NULL || cap == 0)
        return;

    if (relative == NULL) {
        out[0] = '\0';
        return;
    }

    if (is_absolute(relative)) {
        snprintf(out, cap, "%s", relative);
        return;
    }

    /* Сначала как есть — от текущей папки. */
    if (hello_file_exists(relative)) {
        snprintf(out, cap, "%s", relative);
        return;
    }

    const char *base = hello_base_path();
    if (base[0] != '\0') {
        snprintf(out, cap, "%s%s", base, relative);
        if (hello_file_exists(out))
            return;
    }

    /* Не нашли — возвращаем исходный вариант: пусть вызывающий
     * покажет понятную ошибку с тем путём, что просили. */
    snprintf(out, cap, "%s", relative);
}

static bool try_font(const char *candidate, char *out, size_t cap)
{
    if (candidate == NULL || candidate[0] == '\0')
        return false;

    char resolved[CONFIG_PATH_MAX * 2];
    hello_resolve_path(candidate, resolved, sizeof(resolved));

    if (hello_file_exists(resolved)) {
        snprintf(out, cap, "%s", resolved);
        return true;
    }
    return false;
}

bool hello_find_font(const Config *cfg, char *out, size_t cap)
{
    if (cfg == NULL || out == NULL || cap == 0)
        return false;

    if (try_font(cfg->font_path, out, cap))
        return true;

    static const char *const fallbacks[] = {
        "assets/fonts/DejaVuSansMono-Bold.ttf",
        "../assets/fonts/DejaVuSansMono-Bold.ttf",
        "DejaVuSansMono-Bold/DejaVuSansMono-Bold.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/lucon.ttf",
        "C:/Windows/Fonts/courbd.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
        "/usr/share/fonts/dejavu/DejaVuSansMono-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/Library/Fonts/DejaVuSansMono-Bold.ttf",
    };

    for (size_t i = 0; i < sizeof(fallbacks) / sizeof(fallbacks[0]); ++i) {
        if (try_font(fallbacks[i], out, cap))
            return true;
    }
    return false;
}

void hello_set_window_icon(SDL_Window *window)
{
    if (window == NULL)
        return;

    SDL_Surface *icon = SDL_CreateRGBSurfaceWithFormatFrom(
        (void *)kIconRgba64, 64, 64, 32, 64 * 4, SDL_PIXELFORMAT_RGBA32);
    if (icon == NULL)
        return;
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

bool hello_open_window(const Config *cfg, bool screenshot, const char *title,
                       SDL_Window **out_window, SDL_Renderer **out_renderer)
{
    if (cfg == NULL || out_window == NULL || out_renderer == NULL)
        return false;

    *out_window = NULL;
    *out_renderer = NULL;

    Uint32 flags = SDL_WINDOW_RESIZABLE;
    flags |= screenshot ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN;

    int x = cfg->window_x;
    int y = cfg->window_y;
    if (x == CONFIG_WINDOW_POS_ANY)
        x = SDL_WINDOWPOS_CENTERED;
    if (y == CONFIG_WINDOW_POS_ANY)
        y = SDL_WINDOWPOS_CENTERED;

    SDL_Window *window = SDL_CreateWindow(title, x, y, cfg->width, cfg->height,
                                          flags);
    if (window == NULL) {
        hello_log("SDL_CreateWindow: %s", SDL_GetError());
        return false;
    }

    Uint32 renderer_flags = screenshot ? SDL_RENDERER_SOFTWARE
                                       : SDL_RENDERER_ACCELERATED;
    if (!screenshot && cfg->vsync)
        renderer_flags |= SDL_RENDERER_PRESENTVSYNC;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, renderer_flags);
    if (renderer == NULL)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        hello_log("SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (!screenshot)
        hello_set_window_icon(window);

    *out_window = window;
    *out_renderer = renderer;
    return true;
}

void hello_close_window(SDL_Window *window, SDL_Renderer *renderer)
{
    if (renderer != NULL)
        SDL_DestroyRenderer(renderer);
    if (window != NULL)
        SDL_DestroyWindow(window);
}
