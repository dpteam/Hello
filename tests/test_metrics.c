#include "test.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "text_metrics.h"

#define N 64

static size_t norm(const char *s, uint32_t *out)
{
    return tm_normalize(s, out, N);
}

static void test_normalize(void)
{
    uint32_t cps[N];

    CHECK_INT_EQ(norm("", cps), 0);
    CHECK_INT_EQ(norm("   ", cps), 0);
    CHECK_INT_EQ(norm("?!.,", cps), 0);

    size_t n = norm("Привет, мир!", cps);
    CHECK_INT_EQ(n, 9); /* приветмир */

    /* Всё в нижнем регистре. */
    CHECK(cps[0] == 0x43F); /* п */
    CHECK(cps[5] == 0x442); /* т */

    /* Буквы и цифры остаются. */
    n = norm("abc123", cps);
    CHECK_INT_EQ(n, 6);
}

static void test_dice(void)
{
    uint32_t a[N], b[N];
    size_t na = norm("привет", a);
    size_t nb = norm("привет", b);

    CHECK_NEAR(tm_dice_bigrams(a, na, b, nb), 1.0f, 0.001);

    nb = norm("приветик", b);
    float partial = tm_dice_bigrams(a, na, b, nb);
    CHECK(partial > 0.5f && partial < 1.0f);

    nb = norm("трактор", b);
    CHECK_NEAR(tm_dice_bigrams(a, na, b, nb), 0.0f, 0.001);

    /* Пустые — ноль. */
    CHECK_NEAR(tm_dice_bigrams(a, 0, b, nb), 0.0f, 0.001);
}

static void test_jaccard(void)
{
    uint32_t a[N], b[N];
    size_t na = norm("как дела", a);
    size_t nb = norm("как дела", b);
    CHECK_NEAR(tm_jaccard_bigrams(a, na, b, nb), 1.0f, 0.001);

    nb = norm("как делишки", b);
    float j = tm_jaccard_bigrams(a, na, b, nb);
    CHECK(j > 0.2f && j < 1.0f);

    /* Жаккар жёстче Дайса на частичном совпадении. */
    CHECK(tm_jaccard_bigrams(a, na, b, nb) <=
          tm_dice_bigrams(a, na, b, nb) + 0.001f);
}

static void test_cosine(void)
{
    uint32_t a[N], b[N];
    size_t na = norm("привет мир", a);
    size_t nb = norm("привет мир", b);
    CHECK_NEAR(tm_cosine_unigrams(a, na, b, nb), 1.0f, 0.001);

    nb = norm("привет", b);
    float c = tm_cosine_unigrams(a, na, b, nb);
    CHECK(c > 0.6f && c < 1.0f);

    /* Общие буквы дают ненулевой, но низкий балл. */
    nb = norm("абракадабра", b);
    float weak = tm_cosine_unigrams(a, na, b, nb);
    CHECK(weak > 0.0f && weak < 0.3f);
}

static void test_levenshtein(void)
{
    uint32_t a[N], b[N];
    size_t na, nb;

    na = norm("кот", a);
    nb = norm("кот", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, (size_t)-1), 0);

    nb = norm("кит", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, (size_t)-1), 1);

    nb = norm("котик", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, (size_t)-1), 2);

    na = norm("", a);
    nb = norm("привет", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, (size_t)-1), 6);

    /* Ранний выход по лимиту. */
    na = norm("abcdef", a);
    nb = norm("uvwxyz", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, 2), 3); /* 2+1 = «хуже лимита» */

    /* Симметричность. */
    na = norm("привет", a);
    nb = norm("превет", b);
    CHECK_INT_EQ(tm_levenshtein(a, na, b, nb, (size_t)-1),
                 tm_levenshtein(b, nb, a, na, (size_t)-1));
}

static void test_damerau(void)
{
    uint32_t a[N], b[N];

    /* Перестановка соседних букв — 1 операция, а не 2. */
    size_t na = norm("форма", a);
    size_t nb = norm("фрома", b);

    size_t plain = tm_levenshtein(a, na, b, nb, (size_t)-1);
    size_t dam = tm_damerau(a, na, b, nb, (size_t)-1);

    CHECK_INT_EQ(plain, 2);
    CHECK_INT_EQ(dam, 1);
    CHECK(dam < plain);

    /* Одинаковые строки. */
    nb = norm("форма", b);
    CHECK_INT_EQ(tm_damerau(a, na, b, nb, (size_t)-1), 0);
}

static void test_jaro_winkler(void)
{
    uint32_t a[N], b[N];
    size_t na = norm("марта", a);
    size_t nb = norm("марта", b);
    CHECK_NEAR(tm_jaro_winkler(a, na, b, nb), 1.0f, 0.001);

    /* Общий префикс даёт высокий балл даже при хвосте. */
    nb = norm("мартышка", b);
    float jw = tm_jaro_winkler(a, na, b, nb);
    CHECK(jw > 0.7f);

    /* Совсем разные. */
    nb = norm("трактор", b);
    CHECK(tm_jaro_winkler(a, na, b, nb) < 0.6f);

    /* Пустые. */
    CHECK_NEAR(tm_jaro_winkler(a, 0, b, 0), 1.0f, 0.001);
    CHECK_NEAR(tm_jaro_winkler(a, na, b, 0), 0.0f, 0.001);
}

static void test_entropy(void)
{
    double uniform[4] = {0.25, 0.25, 0.25, 0.25};
    CHECK_NEAR(tm_entropy(uniform, 4), 2.0, 0.001);

    double certain[3] = {1.0, 0.0, 0.0};
    CHECK_NEAR(tm_entropy(certain, 3), 0.0, 0.001);

    double half[2] = {0.5, 0.5};
    CHECK_NEAR(tm_entropy(half, 2), 1.0, 0.001);

    /* Максимум энтропии — у равномерного. */
    double skewed[4] = {0.7, 0.1, 0.1, 0.1};
    CHECK(tm_entropy(skewed, 4) < tm_entropy(uniform, 4));
}

static void test_softmax(void)
{
    double scores[3] = {1.0, 2.0, 3.0};
    double out[3];

    tm_softmax(scores, 3, 1.0, out);

    double sum = out[0] + out[1] + out[2];
    CHECK_NEAR(sum, 1.0, 0.0001);
    CHECK(out[2] > out[1]);
    CHECK(out[1] > out[0]);

    /* Низкая температура — почти one-hot. */
    tm_softmax(scores, 3, 0.1, out);
    CHECK(out[2] > 0.99);

    /* Высокая — почти равномерно. */
    tm_softmax(scores, 3, 100.0, out);
    CHECK_NEAR(out[0], 1.0 / 3.0, 0.01);

    /* Устойчивость к большим числам (без переполнения). */
    double big[2] = {1000.0, 1001.0};
    tm_softmax(big, 2, 1.0, out);
    CHECK_NEAR(out[0] + out[1], 1.0, 0.0001);
    CHECK(out[1] > out[0]);
}

static void test_power_dist(void)
{
    double weights[3] = {1.0, 2.0, 4.0};
    double out[3];

    tm_power_dist(weights, 3, 1.0, out);
    double sum = out[0] + out[1] + out[2];
    CHECK_NEAR(sum, 1.0, 0.0001);
    CHECK(out[2] > out[1]);
    CHECK(out[1] > out[0]);

    /* Нулевой вес — нулевая вероятность. */
    double zeros[3] = {0.0, 0.0, 5.0};
    tm_power_dist(zeros, 3, 1.0, out);
    CHECK_NEAR(out[0], 0.0, 0.0001);
    CHECK_NEAR(out[1], 0.0, 0.0001);
    CHECK_NEAR(out[2], 1.0, 0.0001);

    /* Всё нули — равномерно, без деления на ноль. */
    double none[2] = {0.0, 0.0};
    tm_power_dist(none, 2, 1.0, out);
    CHECK_NEAR(out[0], 0.5, 0.0001);
}

void test_metrics(void)
{
    test_normalize();
    test_dice();
    test_jaccard();
    test_cosine();
    test_levenshtein();
    test_damerau();
    test_jaro_winkler();
    test_entropy();
    test_softmax();
    test_power_dist();
}
