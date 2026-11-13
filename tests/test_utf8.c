#include "test.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

static void test_sequence_length(void)
{
    CHECK_INT_EQ(utf8_sequence_length('A'), 1);
    CHECK_INT_EQ(utf8_sequence_length(0x7F), 1);
    CHECK_INT_EQ(utf8_sequence_length(0x80), 0); /* продолжение */
    CHECK_INT_EQ(utf8_sequence_length(0xC0), 0); /* overlong */
    CHECK_INT_EQ(utf8_sequence_length(0xC2), 2);
    CHECK_INT_EQ(utf8_sequence_length(0xDF), 2);
    CHECK_INT_EQ(utf8_sequence_length(0xE0), 3);
    CHECK_INT_EQ(utf8_sequence_length(0xEF), 3);
    CHECK_INT_EQ(utf8_sequence_length(0xF0), 4);
    CHECK_INT_EQ(utf8_sequence_length(0xF4), 4);
    CHECK_INT_EQ(utf8_sequence_length(0xF5), 0);
    CHECK_INT_EQ(utf8_sequence_length(0xFF), 0);
}

static void test_decode_basic(void)
{
    uint32_t cp = 0;

    CHECK_INT_EQ(utf8_decode("A", 1, &cp), 1);
    CHECK_INT_EQ(cp, 'A');

    CHECK_INT_EQ(utf8_decode("é", 2, &cp), 2);
    CHECK_INT_EQ(cp, 0xE9);

    CHECK_INT_EQ(utf8_decode("ж", 2, &cp), 2);
    CHECK_INT_EQ(cp, 0x436);

    /* € — три байта */
    CHECK_INT_EQ(utf8_decode("€", 3, &cp), 3);
    CHECK_INT_EQ(cp, 0x20AC);

    /* 😀 — четыре байта */
    CHECK_INT_EQ(utf8_decode("😀", 4, &cp), 4);
    CHECK_INT_EQ(cp, 0x1F600);

    CHECK_INT_EQ(utf8_decode("", 0, &cp), 0);
}

static void test_decode_invalid(void)
{
    uint32_t cp = 0;

    /* Обрезанная последовательность */
    CHECK_INT_EQ(utf8_decode("\xD0", 1, &cp), -1);

    /* Плохой продолжение-байт */
    CHECK_INT_EQ(utf8_decode("\xD0\x41", 2, &cp), -1);

    /* Overlong: C0 AF */
    CHECK_INT_EQ(utf8_decode("\xC0\xAF", 2, &cp), -1);

    /* Суррогат ED A0 80 */
    CHECK_INT_EQ(utf8_decode("\xED\xA0\x80", 3, &cp), -1);

    /* Выше U+10FFFF: F5 ... */
    CHECK_INT_EQ(utf8_decode("\xF5\x80\x80\x80", 4, &cp), -1);

    /* Одинокий continuation */
    CHECK_INT_EQ(utf8_decode("\x80", 1, &cp), -1);
}

static void test_encode_roundtrip(void)
{
    static const uint32_t samples[] = {
        0x0001, 0x0041, 0x007F, 0x0080, 0x00E9, 0x0436,
        0x07FF, 0x0800, 0x20AC, 0xD7FF, 0xE000, 0xFFFD,
        0x10000, 0x1F600, 0x10FFFF,
    };

    for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        char buf[4];
        int n = utf8_encode(samples[i], buf);
        CHECK(n >= 1 && n <= 4);

        uint32_t back = 0;
        int m = utf8_decode(buf, (size_t)n, &back);
        CHECK_INT_EQ(m, n);
        CHECK(back == samples[i]);
    }

    char buf[4];
    CHECK_INT_EQ(utf8_encode(0xD800, buf), -1);
    CHECK_INT_EQ(utf8_encode(0xDFFF, buf), -1);
    CHECK_INT_EQ(utf8_encode(0x110000, buf), -1);
}

static void test_strlen(void)
{
    CHECK_INT_EQ(utf8_strlen(""), 0);
    CHECK_INT_EQ(utf8_strlen("abc"), 3);
    CHECK_INT_EQ(utf8_strlen("привет"), 6);
    CHECK_INT_EQ(utf8_strlen("aж€😀"), 4);
    CHECK_INT_EQ(utf8_strlen(NULL), 0);

    /* Битые байты считаются как один символ каждый. */
    CHECK_INT_EQ(utf8_strlen("\xD0"), 1);
    CHECK_INT_EQ(utf8_strlen("a\x80" "b"), 3);
}

static void test_valid(void)
{
    CHECK(utf8_valid(""));
    CHECK(utf8_valid("hello"));
    CHECK(utf8_valid("Привет, мир! 123"));
    CHECK(utf8_valid("日本語"));

    CHECK(!utf8_valid("\xD0"));
    CHECK(!utf8_valid("\xC0\xAF"));
    CHECK(!utf8_valid(NULL));
}

static void test_prev_next(void)
{
    const char *s = "aж€😀";

    CHECK_INT_EQ(utf8_next(s, 0), 1);  /* a */
    CHECK_INT_EQ(utf8_next(s, 1), 3);  /* ж */
    CHECK_INT_EQ(utf8_next(s, 3), 6);  /* € */
    CHECK_INT_EQ(utf8_next(s, 6), 10); /* 😀 */
    CHECK_INT_EQ(utf8_next(s, 10), 10);

    CHECK_INT_EQ(utf8_prev(s, 10), 6);
    CHECK_INT_EQ(utf8_prev(s, 6), 3);
    CHECK_INT_EQ(utf8_prev(s, 3), 1);
    CHECK_INT_EQ(utf8_prev(s, 1), 0);
    CHECK_INT_EQ(utf8_prev(s, 0), 0);
}

static void test_clip(void)
{
    /* "ж" занимает 2 байта: обрезка по 1 байту должна дать 0. */
    CHECK_INT_EQ(utf8_clip("ж", 1), 0);
    CHECK_INT_EQ(utf8_clip("ж", 2), 2);

    /* "€" 3 байта */
    CHECK_INT_EQ(utf8_clip("a€", 2), 1);
    CHECK_INT_EQ(utf8_clip("a€", 3), 1);
    CHECK_INT_EQ(utf8_clip("a€", 4), 4);

    CHECK_INT_EQ(utf8_clip("abc", 10), 3);
}

static void test_casefold(void)
{
    CHECK(utf8_casefold('A') == 'a');
    CHECK(utf8_casefold('z') == 'z');

    /* Привет → привет */
    CHECK(utf8_casefold(0x41F) == 0x43F);
    CHECK(utf8_casefold(0x401) == 0x451);
    CHECK(utf8_casefold(0x410) == 0x430);

    /* Латинская диакритика */
    CHECK(utf8_casefold(0xC9) == 0xE9);
    CHECK(utf8_casefold(0x130) == 0x131 || utf8_casefold(0x130) == 0x130);
}

static void test_noise(void)
{
    CHECK(utf8_is_noise(' '));
    CHECK(utf8_is_noise('\n'));
    CHECK(utf8_is_noise('\t'));
    CHECK(utf8_is_noise('!'));
    CHECK(utf8_is_noise('?'));
    CHECK(utf8_is_noise(','));
    CHECK(utf8_is_noise('.'));
    CHECK(utf8_is_noise(0x2014)); /* — */
    CHECK(utf8_is_noise(0x00AB)); /* « */

    CHECK(!utf8_is_noise('a'));
    CHECK(!utf8_is_noise(0x44F)); /* я */
    CHECK(!utf8_is_noise('5'));
}

static void test_buffer_append(void)
{
    char *buf = NULL;
    size_t len = 0, cap = 0;

    CHECK(utf8_buf_append(&buf, &len, &cap, "Привет", (size_t)-1));
    CHECK_STR_EQ(buf, "Привет");
    CHECK_INT_EQ(len, 12);

    CHECK(utf8_buf_append(&buf, &len, &cap, ", мир", (size_t)-1));
    CHECK_STR_EQ(buf, "Привет, мир");
    CHECK_INT_EQ(len, 20);

    CHECK(utf8_buf_append(&buf, &len, &cap, "!", 1));
    CHECK_STR_EQ(buf, "Привет, мир!");

    free(buf);
}

static void test_dup(void)
{
    char *copy = utf8_dup("тест");
    CHECK(copy != NULL);
    CHECK_STR_EQ(copy, "тест");
    free(copy);

    char *part = utf8_dup_range("abcdef", 3);
    CHECK_STR_EQ(part, "abc");
    free(part);

    CHECK(utf8_dup(NULL) == NULL);
}

void test_utf8(void)
{
    test_sequence_length();
    test_decode_basic();
    test_decode_invalid();
    test_encode_roundtrip();
    test_strlen();
    test_valid();
    test_prev_next();
    test_clip();
    test_casefold();
    test_noise();
    test_buffer_append();
    test_dup();
}
