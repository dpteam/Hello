#include "test.h"

#include <stdio.h>
#include <string.h>

#include "theme.h"

static void test_builtin(void)
{
    CHECK(theme_count() >= 4);

    const Theme *green = theme_by_name("green");
    CHECK(green != NULL);
    CHECK_STR_EQ(green->name, "green");
    CHECK(green->system.g > green->system.r); /* зелёный */

    CHECK(theme_by_name("amber") != NULL);
    CHECK(theme_by_name("ice") != NULL);
    CHECK(theme_by_name("paper") != NULL);
    CHECK(theme_by_name("нет_такой") == NULL);
    CHECK(theme_by_name(NULL) == NULL);

    CHECK(theme_at(0) != NULL);
    CHECK(theme_at(-1) == NULL);
    CHECK(theme_at(theme_count()) == NULL);

    CHECK(theme_default() != NULL);
    CHECK_STR_EQ(theme_default()->name, "green");
}

static void test_roles(void)
{
    Theme theme;
    theme_copy(&theme, theme_default());

    SDL_Color *bg = theme_role(&theme, "bg");
    CHECK(bg != NULL);
    CHECK_INT_EQ(bg->r, 0);

    SDL_Color *bot = theme_role(&theme, "bot");
    CHECK(bot != NULL);
    CHECK(bot->g > 0);

    CHECK(theme_role(&theme, "logo_a") != NULL);
    CHECK(theme_role(&theme, "logo_c") != NULL);
    CHECK(theme_role(&theme, "selection") != NULL);
    CHECK(theme_role(&theme, "нет_роли") == NULL);
    CHECK(theme_role(NULL, "bg") == NULL);
    CHECK(theme_role(&theme, NULL) == NULL);

    /* Каждая роль из списка находится. */
    for (size_t i = 0; i < theme_role_count(); ++i) {
        const char *name = theme_role_name(i);
        CHECK(name != NULL);
        CHECK(theme_role(&theme, name) != NULL);
    }
    CHECK(theme_role_name(theme_role_count()) == NULL);
}

static void test_copy(void)
{
    Theme a;
    theme_copy(&a, theme_default());

    /* Меняем копию — встроенная тема не должна измениться. */
    a.bg.r = 200;
    const Theme *orig = theme_default();
    CHECK_INT_EQ(orig->bg.r, 0);

    /* Копирование на себя не ломает. */
    theme_copy(&a, &a);
    CHECK_INT_EQ(a.bg.r, 200);
}

static void test_save_load(void)
{
    const char *path = "test_theme.tmp";

    Theme custom;
    theme_copy(&custom, theme_by_name("ice"));
    snprintf(custom.name, sizeof(custom.name), "custom");
    custom.bg.r = 11;
    custom.bg.g = 22;
    custom.bg.b = 33;
    custom.bot.r = 200;

    CHECK(theme_save(&custom, path));

    Theme loaded;
    theme_copy(&loaded, theme_default());
    CHECK(theme_load(path, &loaded));

    CHECK_STR_EQ(loaded.name, "custom");
    CHECK_INT_EQ(loaded.bg.r, 11);
    CHECK_INT_EQ(loaded.bg.g, 22);
    CHECK_INT_EQ(loaded.bg.b, 33);
    CHECK_INT_EQ(loaded.bot.r, 200);

    /* Незаписанные поля остались от базы (ice). */
    CHECK(loaded.user.g > 200);

    remove(path);

    CHECK(!theme_load("нет_файла_12345", &loaded));
    CHECK(!theme_save(NULL, path));
    CHECK(!theme_save(&custom, NULL));
}

void test_theme(void)
{
    test_builtin();
    test_roles();
    test_copy();
    test_save_load();
}
