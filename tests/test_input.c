#include "test.h"

#include <string.h>

#include "input.h"

static void test_init_clear(void)
{
    Input in;
    input_init(&in);

    CHECK_STR_EQ(input_text(&in), "");
    CHECK_INT_EQ(input_cursor(&in), 0);
    CHECK_INT_EQ(input_history_count(&in), 0);

    CHECK(input_insert(&in, "привет"));
    CHECK_STR_EQ(input_text(&in), "привет");
    CHECK_INT_EQ(input_cursor(&in), 12);

    input_clear(&in);
    CHECK_STR_EQ(input_text(&in), "");
    CHECK_INT_EQ(input_cursor(&in), 0);

    input_free(&in);
}

static void test_insert_middle(void)
{
    Input in;
    input_init(&in);

    input_insert(&in, "мир");
    input_home(&in);
    input_insert(&in, "привет ");

    CHECK_STR_EQ(input_text(&in), "привет мир");
    CHECK_INT_EQ(input_cursor(&in), 13);

    input_free(&in);
}

static void test_backspace_utf8(void)
{
    Input in;
    input_init(&in);

    input_insert(&in, "aж€");
    CHECK_INT_EQ(input_cursor(&in), 6);

    CHECK(input_backspace(&in)); /* убираем € */
    CHECK_STR_EQ(input_text(&in), "aж");
    CHECK_INT_EQ(input_cursor(&in), 3);

    CHECK(input_backspace(&in)); /* убираем ж */
    CHECK_STR_EQ(input_text(&in), "a");

    CHECK(input_backspace(&in));
    CHECK_STR_EQ(input_text(&in), "");

    /* Дальше некуда. */
    CHECK(!input_backspace(&in));

    input_free(&in);
}

static void test_delete(void)
{
    Input in;
    input_init(&in);

    input_insert(&in, "aжb");
    input_home(&in);

    CHECK(input_delete(&in)); /* удаляем 'a' */
    CHECK_STR_EQ(input_text(&in), "жb");

    CHECK(input_delete(&in)); /* удаляем 'ж' целиком */
    CHECK_STR_EQ(input_text(&in), "b");

    CHECK(input_delete(&in));
    CHECK_STR_EQ(input_text(&in), "");
    CHECK(!input_delete(&in));

    input_free(&in);
}

static void test_movement(void)
{
    Input in;
    input_init(&in);

    input_insert(&in, "abc");

    CHECK(input_move_left(&in));
    CHECK_INT_EQ(input_cursor(&in), 2);
    CHECK(input_move_left(&in));
    CHECK(input_move_left(&in));
    CHECK_INT_EQ(input_cursor(&in), 0);
    CHECK(!input_move_left(&in));

    CHECK(input_move_right(&in));
    CHECK_INT_EQ(input_cursor(&in), 1);

    input_end(&in);
    CHECK_INT_EQ(input_cursor(&in), 3);
    CHECK(!input_move_right(&in));

    input_home(&in);
    CHECK_INT_EQ(input_cursor(&in), 0);

    input_free(&in);
}

static void test_limit(void)
{
    Input in;
    input_init(&in);

    char big[1024];
    memset(big, 'x', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';

    CHECK(input_insert(&in, big));
    CHECK(strlen(input_text(&in)) <= INPUT_MAX_BYTES);

    /* Вставка не должна разорвать UTF-8: ставим кириллицу на предел. */
    input_clear(&in);
    char cyr[600];
    size_t pos = 0;
    while (pos + 2 < sizeof(cyr)) {
        cyr[pos++] = (char)0xD0;
        cyr[pos++] = (char)0xB0;
    }
    cyr[pos] = '\0';

    input_insert(&in, cyr);
    CHECK(strlen(input_text(&in)) <= INPUT_MAX_BYTES);

    input_free(&in);
}

static void test_history(void)
{
    Input in;
    input_init(&in);

    CHECK(!input_remember(&in, ""));
    CHECK(!input_remember(&in, "    "));

    CHECK(input_remember(&in, "первая"));
    CHECK(input_remember(&in, "вторая"));
    /* Дубликат подряд не добавляется. */
    CHECK(!input_remember(&in, "вторая"));

    CHECK_INT_EQ(input_history_count(&in), 2);
    CHECK_STR_EQ(input_history_at(&in, 0), "первая");
    CHECK_STR_EQ(input_history_at(&in, 1), "вторая");
    CHECK(input_history_at(&in, 5) == NULL);

    /* Навигация: черновик сохраняется и восстанавливается. */
    input_insert(&in, "черновик");

    CHECK(input_history_prev(&in));
    CHECK_STR_EQ(input_text(&in), "вторая");

    CHECK(input_history_prev(&in));
    CHECK_STR_EQ(input_text(&in), "первая");

    CHECK(!input_history_prev(&in));

    CHECK(input_history_next(&in));
    CHECK_STR_EQ(input_text(&in), "вторая");

    CHECK(input_history_next(&in));
    CHECK_STR_EQ(input_text(&in), "черновик");

    CHECK(!input_history_next(&in));

    input_free(&in);
}

static void test_history_trims(void)
{
    Input in;
    input_init(&in);

    CHECK(input_remember(&in, "  обрезано  "));
    CHECK_STR_EQ(input_history_at(&in, 0), "обрезано");

    input_free(&in);
}

void test_input(void)
{
    test_init_clear();
    test_insert_middle();
    test_backspace_utf8();
    test_delete();
    test_movement();
    test_limit();
    test_history();
    test_history_trims();
}
