#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"

static void test_seed(void)
{
    Chat *chat = chat_create(1);
    CHECK(chat != NULL);
    CHECK_INT_EQ(chat_count(chat), 23);
    CHECK_INT_EQ(chat_learned_count(chat), 0);
    CHECK_STR_EQ(chat_phrase(chat, 0), "Привет");
    CHECK(chat_phrase(chat, 999) == NULL);
    chat_destroy(chat);
}

static void test_learn(void)
{
    Chat *chat = chat_create(1);

    CHECK(chat_learn(chat, "Я люблю котиков"));
    CHECK_INT_EQ(chat_count(chat), 24);
    CHECK_INT_EQ(chat_learned_count(chat), 1);

    /* Дубликат, в том числе в другом регистре, отклоняется. */
    CHECK(!chat_learn(chat, "Я люблю котиков"));
    CHECK(!chat_learn(chat, "я ЛЮБЛЮ котиков"));
    CHECK_INT_EQ(chat_count(chat), 24);

    /* Пустые и пробельные — мимо. */
    CHECK(!chat_learn(chat, ""));
    CHECK(!chat_learn(chat, "   \t  "));
    CHECK(!chat_learn(chat, NULL));

    /* Обрезка пробелов сохраняется. */
    CHECK(chat_learn(chat, "   хвост   "));
    CHECK_STR_EQ(chat_phrase(chat, chat_count(chat) - 1), "хвост");

    chat_forget(chat);
    CHECK_INT_EQ(chat_count(chat), 23);
    CHECK_INT_EQ(chat_learned_count(chat), 0);

    chat_destroy(chat);
}

static void test_similarity(void)
{
    /* Идентичные строки. */
    CHECK_NEAR(chat_similarity("привет", "привет"), 1.0f, 0.001);

    /* Регистр и пунктуация не мешают. */
    float a = chat_similarity("Привет!", "привет");
    CHECK_NEAR(a, 1.0f, 0.001);

    /* Совсем разные — низко. */
    CHECK(chat_similarity("привет", "трактор") < 0.25f);

    /* Симметричность. */
    float ab = chat_similarity("как дела", "как делишки");
    float ba = chat_similarity("как делишки", "как дела");
    CHECK_NEAR(ab, ba, 0.0001);

    /* Подстрока даёт заметный бонус. */
    CHECK(chat_similarity("дела", "как дела") > 0.5f);

    /* Пустое — ноль. */
    CHECK_NEAR(chat_similarity("", "привет"), 0.0f, 0.001);
    CHECK_NEAR(chat_similarity("!!!", "???"), 0.0f, 0.001);
}

static void test_reply_prefers_learned(void)
{
    Chat *chat = chat_create(1234);
    CHECK(chat_learn(chat, "Я программист"));
    CHECK(chat_learn(chat, "Люблю кофе"));

    /* Ответ на почти точное совпадение должен быть именно этой фразой. */
    for (int i = 0; i < 20; ++i) {
        const char *reply = chat_reply(chat, "я программист");
        CHECK(reply != NULL);
        CHECK(strstr(reply, "программист") != NULL);
    }

    chat_destroy(chat);
}

static void test_reply_never_null(void)
{
    Chat *chat = chat_create(7);

    const char *inputs[] = {
        "", "   ", "?!", "привет", "как дела?", "аааааааааааааааааааа",
        "日本語のテキスト", "😀😀😀", "/help",
    };

    for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i) {
        const char *reply = chat_reply(chat, inputs[i]);
        CHECK(reply != NULL);
        CHECK(reply[0] != '\0');
    }

    chat_destroy(chat);
}

static void test_reply_deterministic_with_seed(void)
{
    Chat *a = chat_create(42);
    Chat *b = chat_create(42);

    for (int i = 0; i < 30; ++i) {
        const char *ra = chat_reply(a, "расскажи что-нибудь");
        const char *rb = chat_reply(b, "расскажи что-нибудь");
        CHECK_STR_EQ(ra, rb);
    }

    chat_destroy(a);
    chat_destroy(b);
}

static void test_no_immediate_repeat(void)
{
    Chat *chat = chat_create(99);
    const char *prev = NULL;
    int repeats = 0;

    for (int i = 0; i < 60; ++i) {
        const char *reply = chat_reply(chat, "qwerty");
        if (prev != NULL && strcmp(prev, reply) == 0)
            ++repeats;
        prev = reply;
    }

    /* Иногда повтор возможен, но не подряд же 60 раз. */
    CHECK(repeats < 20);
    chat_destroy(chat);
}

static void test_save_load_roundtrip(void)
{
    const char *path = "test_chat_memory.tmp";

    Chat *chat = chat_create(5);
    CHECK(chat_learn(chat, "Фраза с \\ обратным слэшем"));
    CHECK(chat_learn(chat, "Обычная фраза"));
    CHECK(chat_save(chat, path));
    chat_destroy(chat);

    Chat *loaded = chat_create(5);
    size_t n = chat_load(loaded, path);
    CHECK_INT_EQ(n, 2);
    CHECK_INT_EQ(chat_count(loaded), 25);
    CHECK_STR_EQ(chat_phrase(loaded, 23), "Фраза с \\ обратным слэшем");
    CHECK_STR_EQ(chat_phrase(loaded, 24), "Обычная фраза");
    chat_destroy(loaded);

    remove(path);

    /* Загрузка отсутствующего файла — не ошибка, просто 0. */
    Chat *empty = chat_create(5);
    CHECK_INT_EQ(chat_load(empty, "definitely_missing_file_12345"), 0);
    CHECK_INT_EQ(chat_count(empty), 23);
    chat_destroy(empty);
}

static void test_random(void)
{
    Chat *chat = chat_create(3);
    for (int i = 0; i < 50; ++i) {
        const char *phrase = chat_random(chat);
        CHECK(phrase != NULL && phrase[0] != '\0');
    }
    chat_destroy(chat);

    CHECK_STR_EQ(chat_random(NULL), "…");
}

void test_chat(void)
{
    test_seed();
    test_learn();
    test_similarity();
    test_reply_prefers_learned();
    test_reply_never_null();
    test_reply_deterministic_with_seed();
    test_no_immediate_repeat();
    test_save_load_roundtrip();
    test_random();
}
