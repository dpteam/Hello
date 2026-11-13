#include "test.h"

#include <string.h>

#include "viewkind.h"

static void test_names(void)
{
    CHECK_STR_EQ(view_kind_name(VIEW_SYSTEM), "system");
    CHECK_STR_EQ(view_kind_name(VIEW_USER), "user");
    CHECK_STR_EQ(view_kind_name(VIEW_BOT), "bot");
    CHECK_STR_EQ(view_kind_name(VIEW_WARNING), "warning");
    CHECK_STR_EQ(view_kind_name(VIEW_ERROR), "error");
    CHECK_STR_EQ(view_kind_name(VIEW_HINT), "hint");

    /* Каждое имя непустое и уникально. */
    for (int a = VIEW_SYSTEM; a <= VIEW_HINT; ++a) {
        const char *na = view_kind_name((ViewKind)a);
        CHECK(na != NULL && na[0] != '\0');
        for (int b = a + 1; b <= VIEW_HINT; ++b) {
            const char *nb = view_kind_name((ViewKind)b);
            CHECK(strcmp(na, nb) != 0);
        }
    }
}

static void test_authors(void)
{
    CHECK_STR_EQ(view_kind_author(VIEW_USER), "Вы");
    CHECK_STR_EQ(view_kind_author(VIEW_BOT), "Hello");

    /* У системных строк автора нет. */
    CHECK(view_kind_author(VIEW_SYSTEM) == NULL);
    CHECK(view_kind_author(VIEW_HINT) == NULL);
    CHECK(view_kind_author(VIEW_WARNING) == NULL);
    CHECK(view_kind_author(VIEW_ERROR) == NULL);
}

static void test_dialogue(void)
{
    CHECK_INT_EQ(view_kind_is_dialogue(VIEW_USER), 1);
    CHECK_INT_EQ(view_kind_is_dialogue(VIEW_BOT), 1);
    CHECK_INT_EQ(view_kind_is_dialogue(VIEW_SYSTEM), 0);
    CHECK_INT_EQ(view_kind_is_dialogue(VIEW_HINT), 0);
}

void test_viewkind(void)
{
    test_names();
    test_authors();
    test_dialogue();
}
