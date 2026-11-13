/*
 * markov.h — цепь Маркова по словам.
 *
 * Идея: бот не просто переставляет выученные фразы, а строит из них
 * новую речь. Состояние — последние (order-1) слов, переходы —
 * наблюдавшиеся продолжения с частотными весами.
 *
 * Уровни:
 *   order 1 — униграммы: «каждое слово тянет за собой следующее»
 *   order 2 — биграммы: контекст из двух слов (по умолчанию)
 *   order 3 — триграммы: заметно связнее, но требует больше данных
 *
 * Сглаживание — «глупый бэкофф» (Brants et al., 2007): если для
 * полного контекста переходов нет, пробуем контекст короче.
 */
#ifndef HELLO_MARKOV_H
#define HELLO_MARKOV_H

#include <stdbool.h>
#include <stddef.h>

#define MARKOV_MAX_ORDER 3
#define MARKOV_DEFAULT_ORDER 2

typedef struct Markov Markov;

/* order — 1..MARKOV_MAX_ORDER, seed — для воспроизводимости. */
Markov *markov_create(unsigned order, unsigned seed);
void markov_destroy(Markov *model);

/* Разбирает текст на предложения и учит переходы. */
void markov_train(Markov *model, const char *utf8);

/* Забыть всё выученное (словарь и переходы), оставив конфигурацию. */
void markov_reset(Markov *model);

/* Генерирует фразу. prompt может быть NULL или "".
 * temperature: 0.7 — сдержанно, 1.0 — естественно, 1.6 — сумбурно.
 * Возвращает false, если модель пуста. */
bool markov_generate(Markov *model, const char *prompt, double temperature,
                     char *out, size_t cap);

/* Средняя «удивлённость» модели на тексте: 2^H, где H — среднее
 * -log2 p. Меньше — текст привычнее для модели. */
double markov_perplexity(const Markov *model, const char *utf8);

/* Диагностика. */
size_t markov_vocab_size(const Markov *model);
size_t markov_transition_count(const Markov *model);
size_t markov_tokens_seen(const Markov *model);
unsigned markov_order(const Markov *model);

#endif /* HELLO_MARKOV_H */
