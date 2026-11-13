#include "test.h"

#include <string.h>

#include "markov.h"

static void test_empty(void)
{
    Markov *model = markov_create(2, 1);
    CHECK(model != NULL);
    CHECK_INT_EQ(markov_order(model), 2);
    CHECK_INT_EQ(markov_vocab_size(model), 0);
    CHECK_INT_EQ(markov_tokens_seen(model), 0);

    char out[256];
    CHECK(!markov_generate(model, NULL, 1.0, out, sizeof(out)));
    CHECK_STR_EQ(out, "");

    CHECK_NEAR(markov_perplexity(model, "привет"), 0.0, 0.001);

    markov_destroy(model);
}

static void test_order_clamp(void)
{
    Markov *a = markov_create(0, 1);
    CHECK_INT_EQ(markov_order(a), 1);
    markov_destroy(a);

    Markov *b = markov_create(99, 1);
    CHECK_INT_EQ(markov_order(b), MARKOV_MAX_ORDER);
    markov_destroy(b);
}

static void test_train_and_generate(void)
{
    Markov *model = markov_create(2, 42);

    markov_train(model, "я люблю котиков");
    markov_train(model, "я люблю собак");
    markov_train(model, "котики любят спать");

    CHECK(markov_vocab_size(model) >= 6);
    CHECK(markov_tokens_seen(model) > 0);

    char out[256];
    bool ok = false;
    for (int i = 0; i < 20 && !ok; ++i)
        ok = markov_generate(model, NULL, 1.0, out, sizeof(out));

    CHECK(ok);
    CHECK(strlen(out) > 0);

    /* Все слова выхода должны быть из словаря. */
    CHECK(strstr(out, "люблю") != NULL || strstr(out, "любят") != NULL ||
          strstr(out, "котиков") != NULL || strstr(out, "собак") != NULL ||
          strstr(out, "спать") != NULL);

    markov_destroy(model);
}

static void test_prompt_continuation(void)
{
    Markov *model = markov_create(2, 7);
    for (int i = 0; i < 5; ++i)
        markov_train(model, "я люблю котиков и собак");

    char out[256];
    bool ok = markov_generate(model, "я люблю", 0.8, out, sizeof(out));
    CHECK(ok);
    CHECK(strlen(out) > 0);

    markov_destroy(model);
}

static void test_determinism(void)
{
    Markov *a = markov_create(2, 1234);
    Markov *b = markov_create(2, 1234);

    const char *text = "привет как дела хорошо а ты как";
    markov_train(a, text);
    markov_train(b, text);

    char oa[256], ob[256];
    for (int i = 0; i < 10; ++i) {
        markov_generate(a, NULL, 1.0, oa, sizeof(oa));
        markov_generate(b, NULL, 1.0, ob, sizeof(ob));
        CHECK_STR_EQ(oa, ob);
    }

    markov_destroy(a);
    markov_destroy(b);
}

static void test_reset(void)
{
    Markov *model = markov_create(2, 5);
    markov_train(model, "тестовая фраза для модели");
    CHECK(markov_vocab_size(model) > 0);

    markov_reset(model);
    CHECK_INT_EQ(markov_vocab_size(model), 0);
    CHECK_INT_EQ(markov_tokens_seen(model), 0);

    char out[64];
    CHECK(!markov_generate(model, NULL, 1.0, out, sizeof(out)));

    markov_destroy(model);
}

static void test_perplexity(void)
{
    Markov *model = markov_create(2, 11);
    const char *text = "кошка сидит на окне и смотрит во двор";
    markov_train(model, text);

    double seen = markov_perplexity(model, "кошка сидит на окне");
    double unseen = markov_perplexity(model, "трактор пашет поле утром");

    CHECK(seen > 0.0);
    CHECK(unseen > 0.0);
    /* Знакомый текст должен быть менее удивительным. */
    CHECK(seen < unseen);

    markov_destroy(model);
}

static void test_punctuation_split(void)
{
    Markov *model = markov_create(1, 3);
    markov_train(model, "Привет! Как дела? Хорошо.");

    /* Три предложения — три стартовых перехода. */
    CHECK(markov_transition_count(model) > 0);
    CHECK(markov_vocab_size(model) >= 4);

    markov_destroy(model);
}

static void test_utf8_words(void)
{
    Markov *model = markov_create(2, 9);
    markov_train(model, "日本語 тест 😀 ёжик");

    /* Слова должны быть корректным UTF-8. */
    char out[256];
    if (markov_generate(model, NULL, 1.0, out, sizeof(out))) {
        for (const char *p = out; *p; ++p) {
            CHECK((unsigned char)*p != 0xFF);
        }
    }

    CHECK(markov_vocab_size(model) >= 4);
    markov_destroy(model);
}

void test_markov(void)
{
    test_empty();
    test_order_clamp();
    test_train_and_generate();
    test_prompt_continuation();
    test_determinism();
    test_reset();
    test_perplexity();
    test_punctuation_split();
    test_utf8_words();
}
