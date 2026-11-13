#include "theme.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Первая схема — та самая зелёная из 2013-го. */
static const Theme kThemes[] = {
    {
        .name = "green",
        .bg = {0, 0, 0, 255},
        .system = {0, 255, 80, 255},
        .user = {150, 255, 180, 255},
        .bot = {0, 220, 100, 255},
        .warning = {255, 200, 60, 255},
        .error = {255, 90, 60, 255},
        .hint = {90, 170, 120, 255},
        .input = {0, 255, 80, 255},
        .status = {0, 120, 50, 255},
        .caret = {0, 255, 80, 255},
        .separator = {0, 80, 30, 255},
        .selection = {0, 90, 40, 255},
        .logo_a = {255, 60, 60, 255},
        .logo_b = {60, 255, 90, 255},
        .logo_c = {70, 110, 255, 255},
    },
    {
        .name = "amber",
        .bg = {18, 10, 2, 255},
        .system = {255, 176, 0, 255},
        .user = {255, 224, 150, 255},
        .bot = {255, 150, 20, 255},
        .warning = {255, 230, 120, 255},
        .error = {255, 90, 60, 255},
        .hint = {170, 120, 30, 255},
        .input = {255, 196, 40, 255},
        .status = {130, 85, 10, 255},
        .caret = {255, 196, 40, 255},
        .separator = {90, 55, 5, 255},
        .selection = {80, 50, 0, 255},
        .logo_a = {255, 80, 40, 255},
        .logo_b = {255, 200, 60, 255},
        .logo_c = {255, 140, 30, 255},
    },
    {
        .name = "ice",
        .bg = {6, 10, 18, 255},
        .system = {130, 205, 255, 255},
        .user = {205, 232, 255, 255},
        .bot = {95, 170, 255, 255},
        .warning = {255, 214, 130, 255},
        .error = {255, 110, 110, 255},
        .hint = {95, 135, 175, 255},
        .input = {150, 215, 255, 255},
        .status = {60, 90, 125, 255},
        .caret = {150, 215, 255, 255},
        .separator = {30, 50, 75, 255},
        .selection = {25, 55, 95, 255},
        .logo_a = {255, 90, 110, 255},
        .logo_b = {90, 230, 160, 255},
        .logo_c = {110, 170, 255, 255},
    },
    {
        .name = "paper",
        .bg = {246, 244, 238, 255},
        .system = {30, 90, 60, 255},
        .user = {25, 25, 25, 255},
        .bot = {35, 70, 125, 255},
        .warning = {150, 100, 0, 255},
        .error = {170, 35, 35, 255},
        .hint = {120, 118, 112, 255},
        .input = {20, 20, 20, 255},
        .status = {150, 148, 140, 255},
        .caret = {30, 90, 60, 255},
        .separator = {205, 200, 190, 255},
        .selection = {215, 225, 200, 255},
        .logo_a = {200, 60, 60, 255},
        .logo_b = {60, 160, 80, 255},
        .logo_c = {60, 90, 180, 255},
    },
};

/* Порядок должен совпадать с theme_role_name(). */
typedef struct {
    const char *name;
    size_t offset;
} ThemeRole;

#define ROLE(field) {#field, offsetof(Theme, field)}

static const ThemeRole kRoles[] = {
    ROLE(bg),
    ROLE(system),
    ROLE(user),
    ROLE(bot),
    ROLE(warning),
    ROLE(error),
    ROLE(hint),
    ROLE(input),
    ROLE(status),
    ROLE(caret),
    ROLE(separator),
    ROLE(selection),
    ROLE(logo_a),
    ROLE(logo_b),
    ROLE(logo_c),
};

int theme_count(void)
{
    return (int)(sizeof(kThemes) / sizeof(kThemes[0]));
}

const Theme *theme_at(int index)
{
    if (index < 0 || index >= theme_count())
        return NULL;
    return &kThemes[index];
}

const Theme *theme_by_name(const char *name)
{
    if (name == NULL)
        return NULL;

    for (int i = 0; i < theme_count(); ++i) {
        if (strcmp(kThemes[i].name, name) == 0)
            return &kThemes[i];
    }
    return NULL;
}

const Theme *theme_default(void)
{
    return &kThemes[0];
}

void theme_copy(Theme *dst, const Theme *src)
{
    if (dst == NULL || src == NULL)
        return;
    memcpy(dst, src, sizeof(*dst));
}

SDL_Color *theme_role(Theme *theme, const char *role)
{
    if (theme == NULL || role == NULL)
        return NULL;

    for (size_t i = 0; i < sizeof(kRoles) / sizeof(kRoles[0]); ++i) {
        if (strcmp(kRoles[i].name, role) == 0)
            return (SDL_Color *)((char *)theme + kRoles[i].offset);
    }
    return NULL;
}

size_t theme_role_count(void)
{
    return sizeof(kRoles) / sizeof(kRoles[0]);
}

const char *theme_role_name(size_t index)
{
    if (index >= theme_role_count())
        return NULL;
    return kRoles[index].name;
}

bool theme_save(const Theme *theme, const char *path)
{
    if (theme == NULL || path == NULL)
        return false;

    FILE *out = fopen(path, "wb");
    if (out == NULL)
        return false;

    fprintf(out, "# Codename Hello — тема\n");
    fprintf(out, "name = %s\n", theme->name);

    for (size_t i = 0; i < theme_role_count(); ++i) {
        const SDL_Color *c =
            (const SDL_Color *)((const char *)theme + kRoles[i].offset);
        fprintf(out, "%s = %u %u %u %u\n", kRoles[i].name, (unsigned)c->r,
                (unsigned)c->g, (unsigned)c->b, (unsigned)c->a);
    }

    bool ok = !ferror(out);
    if (fclose(out) != 0)
        ok = false;
    return ok;
}

static char *trim(char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
        ++s;

    size_t len = strlen(s);
    while (len > 0) {
        char c = s[len - 1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        s[--len] = '\0';
    }
    return s;
}

bool theme_load(const char *path, Theme *out)
{
    if (path == NULL || out == NULL)
        return false;

    FILE *in = fopen(path, "rb");
    if (in == NULL)
        return false;

    /* Начинаем с текущей темы: чего нет в файле — остаётся как было. */
    char line[256];
    while (fgets(line, (int)sizeof(line), in) != NULL) {
        char *text = trim(line);
        if (*text == '#' || *text == '\0')
            continue;

        char *eq = strchr(text, '=');
        if (eq == NULL)
            continue;

        *eq = '\0';
        char *key = trim(text);
        char *value = trim(eq + 1);

        if (strcmp(key, "name") == 0) {
            snprintf(out->name, sizeof(out->name), "%s", value);
            continue;
        }

        SDL_Color *color = theme_role(out, key);
        if (color == NULL)
            continue;

        unsigned r = 0, g = 0, b = 0, a = 255;
        int parsed = sscanf(value, "%u %u %u %u", &r, &g, &b, &a);
        if (parsed < 3)
            continue;

        color->r = (Uint8)(r > 255 ? 255 : r);
        color->g = (Uint8)(g > 255 ? 255 : g);
        color->b = (Uint8)(b > 255 ? 255 : b);
        color->a = (Uint8)(a > 255 ? 255 : a);
    }

    fclose(in);
    return true;
}
