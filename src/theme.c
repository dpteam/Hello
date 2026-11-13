#include "theme.h"

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
    },
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
