#include "text_metrics.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

#define TM_GRAM_MAX 512

size_t tm_normalize(const char *text, uint32_t *out, size_t max)
{
    if (text == NULL || out == NULL || max == 0)
        return 0;

    size_t len = strlen(text);
    size_t pos = 0;
    size_t n = 0;

    while (pos < len && n < max) {
        uint32_t cp = 0;
        int step = utf8_decode(text + pos, len - pos, &cp);
        if (step <= 0) {
            ++pos;
            continue;
        }
        pos += (size_t)step;

        if (utf8_is_noise(cp))
            continue;

        out[n++] = utf8_casefold(cp);
    }
    return n;
}

static int cmp_u64(const void *lhs, const void *rhs)
{
    uint64_t a = *(const uint64_t *)lhs;
    uint64_t b = *(const uint64_t *)rhs;
    return (a > b) - (a < b);
}

/* Строит биграммы в массив. Возвращает количество.
 * Для строки из одного символа биграмм нет — используем сам символ
 * со старшим битом как маркер, иначе одиночные слова не сравнятся. */
static size_t build_grams(const uint32_t *cps, size_t n, uint64_t *grams)
{
    if (n == 0)
        return 0;
    if (n == 1) {
        grams[0] = 0x8000000000000000ull | (uint64_t)cps[0];
        return 1;
    }
    for (size_t i = 0; i + 1 < n; ++i)
        grams[i] = ((uint64_t)cps[i] << 32) | (uint64_t)cps[i + 1];
    return n - 1;
}

static size_t multiset_common(const uint64_t *a, size_t na,
                              const uint64_t *b, size_t nb)
{
    size_t i = 0, j = 0, common = 0;
    while (i < na && j < nb) {
        if (a[i] == b[j]) {
            ++common;
            ++i;
            ++j;
        } else if (a[i] < b[j]) {
            ++i;
        } else {
            ++j;
        }
    }
    return common;
}

static size_t set_common(const uint64_t *a, size_t na,
                         const uint64_t *b, size_t nb)
{
    size_t i = 0, j = 0, common = 0;
    while (i < na && j < nb) {
        if (a[i] == b[j]) {
            ++common;
            ++i;
            ++j;
        } else if (a[i] < b[j]) {
            ++i;
        } else {
            ++j;
        }
    }
    return common;
}

static size_t dedupe(uint64_t *items, size_t n)
{
    if (n == 0)
        return 0;
    size_t out = 1;
    for (size_t i = 1; i < n; ++i) {
        if (items[i] != items[out - 1])
            items[out++] = items[i];
    }
    return out;
}

float tm_dice_bigrams(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb)
{
    if (na == 0 || nb == 0)
        return 0.0f;

    uint64_t ga[TM_GRAM_MAX];
    uint64_t gb[TM_GRAM_MAX];

    if (na > TM_GRAM_MAX || nb > TM_GRAM_MAX)
        return 0.0f;

    size_t nga = build_grams(a, na, ga);
    size_t ngb = build_grams(b, nb, gb);
    if (nga == 0 || ngb == 0)
        return 0.0f;

    qsort(ga, nga, sizeof(uint64_t), cmp_u64);
    qsort(gb, ngb, sizeof(uint64_t), cmp_u64);

    size_t common = multiset_common(ga, nga, gb, ngb);
    return (float)(2.0 * (double)common / (double)(nga + ngb));
}

float tm_jaccard_bigrams(const uint32_t *a, size_t na,
                         const uint32_t *b, size_t nb)
{
    if (na == 0 || nb == 0 || na > TM_GRAM_MAX || nb > TM_GRAM_MAX)
        return 0.0f;

    uint64_t ga[TM_GRAM_MAX];
    uint64_t gb[TM_GRAM_MAX];

    size_t nga = build_grams(a, na, ga);
    size_t ngb = build_grams(b, nb, gb);
    if (nga == 0 || ngb == 0)
        return 0.0f;

    qsort(ga, nga, sizeof(uint64_t), cmp_u64);
    qsort(gb, ngb, sizeof(uint64_t), cmp_u64);
    nga = dedupe(ga, nga);
    ngb = dedupe(gb, ngb);

    size_t common = set_common(ga, nga, gb, ngb);
    size_t uni = nga + ngb - common;
    if (uni == 0)
        return 0.0f;
    return (float)((double)common / (double)uni);
}

float tm_cosine_unigrams(const uint32_t *a, size_t na,
                         const uint32_t *b, size_t nb)
{
    if (na == 0 || nb == 0)
        return 0.0f;

    /* Косинус по TF-векторам: dot = Σ tf_a·tf_b,
     * норма = sqrt(Σ tf_a²) · sqrt(Σ tf_b²).
     * Каждый кодпоинт учитывается один раз — на первом вхождении. */
    double dot = 0.0;
    double norm_a = 0.0;
    double norm_b = 0.0;

    for (size_t i = 0; i < na; ++i) {
        bool seen = false;
        for (size_t k = 0; k < i; ++k) {
            if (a[k] == a[i]) {
                seen = true;
                break;
            }
        }
        if (seen)
            continue;

        size_t freq_a = 0;
        for (size_t j = 0; j < na; ++j) {
            if (a[j] == a[i])
                ++freq_a;
        }
        norm_a += (double)(freq_a * freq_a);

        size_t freq_b = 0;
        for (size_t j = 0; j < nb; ++j) {
            if (b[j] == a[i])
                ++freq_b;
        }
        dot += (double)(freq_a * freq_b);
    }

    for (size_t j = 0; j < nb; ++j) {
        bool seen = false;
        for (size_t k = 0; k < j; ++k) {
            if (b[k] == b[j]) {
                seen = true;
                break;
            }
        }
        if (seen)
            continue;

        size_t freq_b = 0;
        for (size_t k = 0; k < nb; ++k) {
            if (b[k] == b[j])
                ++freq_b;
        }
        norm_b += (double)(freq_b * freq_b);
    }

    if (dot == 0.0 || norm_a == 0.0 || norm_b == 0.0)
        return 0.0f;

    double denom = sqrt(norm_a) * sqrt(norm_b);
    if (denom == 0.0)
        return 0.0f;

    double value = dot / denom;
    if (value > 1.0)
        value = 1.0;
    if (value < 0.0)
        value = 0.0;
    return (float)value;
}

size_t tm_levenshtein(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb, size_t max_distance)
{
    if (na == 0)
        return nb;
    if (nb == 0)
        return na;

    /* Быстрый отсев по разнице длин. */
    size_t diff = (na > nb) ? (na - nb) : (nb - na);
    if (max_distance != (size_t)-1 && diff > max_distance)
        return max_distance + 1;

    size_t *prev = (size_t *)malloc((nb + 1) * sizeof(size_t));
    size_t *curr = (size_t *)malloc((nb + 1) * sizeof(size_t));
    if (prev == NULL || curr == NULL) {
        free(prev);
        free(curr);
        return (na > nb) ? na : nb;
    }

    for (size_t j = 0; j <= nb; ++j)
        prev[j] = j;

    for (size_t i = 1; i <= na; ++i) {
        curr[0] = i;
        size_t row_min = curr[0];

        for (size_t j = 1; j <= nb; ++j) {
            size_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            size_t del = prev[j] + 1;
            size_t ins = curr[j - 1] + 1;
            size_t sub = prev[j - 1] + cost;

            size_t best = del < ins ? del : ins;
            if (sub < best)
                best = sub;
            curr[j] = best;

            if (best < row_min)
                row_min = best;
        }

        if (max_distance != (size_t)-1 && row_min > max_distance) {
            free(prev);
            free(curr);
            return max_distance + 1;
        }

        size_t *swap = prev;
        prev = curr;
        curr = swap;
    }

    size_t result = prev[nb];
    free(prev);
    free(curr);
    return result;
}

size_t tm_damerau(const uint32_t *a, size_t na,
                  const uint32_t *b, size_t nb, size_t max_distance)
{
    /* Полноценная матрица: нужен доступ к i-2/j-2.
     * Для коротких фраз это дешевле, чем оптимизации. */
    if (na == 0)
        return nb;
    if (nb == 0)
        return na;

    size_t diff = (na > nb) ? (na - nb) : (nb - na);
    if (max_distance != (size_t)-1 && diff > max_distance)
        return max_distance + 1;

    size_t rows = na + 1;
    size_t cols = nb + 1;
    if (rows > 256 || cols > 256)
        return tm_levenshtein(a, na, b, nb, max_distance);

    size_t *d = (size_t *)malloc(rows * cols * sizeof(size_t));
    if (d == NULL)
        return tm_levenshtein(a, na, b, nb, max_distance);

#define D(i, j) d[(i) * cols + (j)]

    for (size_t i = 0; i < rows; ++i)
        D(i, 0) = i;
    for (size_t j = 0; j < cols; ++j)
        D(0, j) = j;

    for (size_t i = 1; i < rows; ++i) {
        for (size_t j = 1; j < cols; ++j) {
            size_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;

            size_t best = D(i - 1, j) + 1;
            if (D(i, j - 1) + 1 < best)
                best = D(i, j - 1) + 1;
            if (D(i - 1, j - 1) + cost < best)
                best = D(i - 1, j - 1) + cost;

            if (i > 1 && j > 1 && a[i - 1] == b[j - 2] && a[i - 2] == b[j - 1]) {
                size_t transposed = D(i - 2, j - 2) + 1;
                if (transposed < best)
                    best = transposed;
            }
            D(i, j) = best;
        }
    }

    size_t result = D(na, nb);
#undef D

    free(d);
    if (max_distance != (size_t)-1 && result > max_distance)
        return max_distance + 1;
    return result;
}

float tm_jaro_winkler(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb)
{
    if (na == 0 && nb == 0)
        return 1.0f;
    if (na == 0 || nb == 0)
        return 0.0f;
    if (na > 256 || nb > 256)
        return 0.0f;

    size_t window = (na > nb ? na : nb) / 2;
    if (window > 0)
        --window;

    bool a_match[256] = {false};
    bool b_match[256] = {false};
    size_t matches = 0;

    for (size_t i = 0; i < na; ++i) {
        size_t start = (i > window) ? i - window : 0;
        size_t end = i + window + 1;
        if (end > nb)
            end = nb;

        for (size_t j = start; j < end; ++j) {
            if (b_match[j] || a[i] != b[j])
                continue;
            a_match[i] = true;
            b_match[j] = true;
            ++matches;
            break;
        }
    }

    if (matches == 0)
        return 0.0f;

    /* Перестановки: сколько совпавших пар идут в разном порядке. */
    size_t transpositions = 0;
    size_t k = 0;
    for (size_t i = 0; i < na; ++i) {
        if (!a_match[i])
            continue;
        while (!b_match[k])
            ++k;
        if (a[i] != b[k])
            ++transpositions;
        ++k;
    }
    transpositions /= 2;

    double m = (double)matches;
    double jaro = (m / (double)na + m / (double)nb +
                   (m - (double)transpositions) / m) / 3.0;

    /* Уинклер: бонус за общий префикс (до 4 символов). */
    size_t prefix = 0;
    while (prefix < 4 && prefix < na && prefix < nb && a[prefix] == b[prefix])
        ++prefix;

    double winkler = jaro + (double)prefix * 0.1 * (1.0 - jaro);
    if (winkler > 1.0)
        winkler = 1.0;
    if (winkler < 0.0)
        winkler = 0.0;
    return (float)winkler;
}

double tm_entropy(const double *p, size_t n)
{
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        if (p[i] <= 0.0)
            continue;
        sum -= p[i] * (log(p[i]) / log(2.0));
    }
    return sum < 0.0 ? 0.0 : sum;
}

void tm_softmax(const double *scores, size_t n, double temperature,
                double *out)
{
    if (n == 0)
        return;
    if (temperature <= 0.0)
        temperature = 1.0;

    double max_score = scores[0];
    for (size_t i = 1; i < n; ++i) {
        if (scores[i] > max_score)
            max_score = scores[i];
    }

    double total = 0.0;
    for (size_t i = 0; i < n; ++i) {
        out[i] = exp((scores[i] - max_score) / temperature);
        total += out[i];
    }

    if (total <= 0.0) {
        for (size_t i = 0; i < n; ++i)
            out[i] = 1.0 / (double)n;
        return;
    }

    for (size_t i = 0; i < n; ++i)
        out[i] /= total;
}

void tm_power_dist(const double *weights, size_t n, double temperature,
                   double *out)
{
    if (n == 0)
        return;
    if (temperature <= 0.0)
        temperature = 1.0;

    double exponent = 1.0 / temperature;
    double total = 0.0;

    for (size_t i = 0; i < n; ++i) {
        double w = weights[i] > 0.0 ? weights[i] : 0.0;
        out[i] = (w > 0.0) ? pow(w, exponent) : 0.0;
        total += out[i];
    }

    if (total <= 0.0) {
        for (size_t i = 0; i < n; ++i)
            out[i] = 1.0 / (double)n;
        return;
    }

    for (size_t i = 0; i < n; ++i)
        out[i] /= total;
}
