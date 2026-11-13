/*
 * text_metrics.h — математика сравнения текста.
 *
 * Все функции работают над массивами кодпоинтов (нормализованный
 * текст без пунктуации и регистра). Это позволяет сравнивать
 * UTF-8 без разбора байтов на каждом шаге.
 */
#ifndef HELLO_TEXT_METRICS_H
#define HELLO_TEXT_METRICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Приводит UTF-8 к массиву кодпоинтов: без «шума» (пробелы,
 * пунктуация), в нижнем регистре. Возвращает количество. */
size_t tm_normalize(const char *text, uint32_t *out, size_t max);

/* --- Коэффициенты похожести множеств ------------------------------- */

/* Коэффициент Дайса по биграммам: 2|A∩B| / (|A|+|B|).
 * Хорош для средних строк, устойчив к перестановкам. */
float tm_dice_bigrams(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb);

/* Коэффициент Жаккара по уникальным биграммам: |A∩B| / |A∪B|.
 * Жёстче Дайса: сильнее штрафует за лишние слова. */
float tm_jaccard_bigrams(const uint32_t *a, size_t na,
                         const uint32_t *b, size_t nb);

/* Косинус по униграммам (TF-векторы). */
float tm_cosine_unigrams(const uint32_t *a, size_t na,
                         const uint32_t *b, size_t nb);

/* --- Расстояния ---------------------------------------------------- */

/* Расстояние Левенштейна по кодпоинтам.
 * max_distance — ранний выход (SIZE_MAX = без ограничения). */
size_t tm_levenshtein(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb, size_t max_distance);

/* Расстояние Дамерау–Левенштейна (учитывает перестановку соседних). */
size_t tm_damerau(const uint32_t *a, size_t na,
                  const uint32_t *b, size_t nb, size_t max_distance);

/* Джаро–Уинклер: чувствителен к общему префиксу, хорош для
 * коротких слов и опечаток. */
float tm_jaro_winkler(const uint32_t *a, size_t na,
                      const uint32_t *b, size_t nb);

/* --- Распределения ------------------------------------------------- */

/* Энтропия Шеннона в битах: -Σ p·log2(p). */
double tm_entropy(const double *p, size_t n);

/* Softmax с температурой: p_i ∝ exp(s_i / T).
 * T < 1 — «острее», T > 1 — «площе». */
void tm_softmax(const double *scores, size_t n, double temperature,
                double *out);

/* Степенное распределение (используется для цепей Маркова):
 * p_i ∝ w_i^(1/T). */
void tm_power_dist(const double *weights, size_t n, double temperature,
                   double *out);

#endif /* HELLO_TEXT_METRICS_H */
