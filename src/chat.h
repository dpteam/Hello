/*
 * chat.h — «мозг» Codename Hello.
 *
 * Идея 2013-го: бот не понимает смысл, он помнит все фразы, которые
 * ему писали, и отвечает той, что ближе к вопросу. Мы сохранили идею
 * и добавили три слоя:
 *
 *   1. Метрики похожести (Дайс, Джаро–Уинклер, Левенштейн) — чтобы
 *      находить близкие фразы и терпеть опечатки.
 *   2. BM25 — чтобы редкие слова весили больше частых.
 *   3. Цепь Маркова — чтобы уметь сочинять новое, а не только
 *      переставлять готовое.
 */
#ifndef HELLO_CHAT_H
#define HELLO_CHAT_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Chat Chat;

/* seed == 0 подставляется детерминированно (1), чтобы тесты были
 * воспроизводимы. */
Chat *chat_create(unsigned seed);
void chat_destroy(Chat *chat);

size_t chat_count(const Chat *chat);
size_t chat_learned_count(const Chat *chat);
const char *chat_phrase(const Chat *chat, size_t index);

/* Добавляет фразу в память. Пустые, слишком длинные и дубликаты
 * (без учёта регистра) игнорируются. При переполнении памяти
 * применяется резервуарная выборка вместо простого отказа. */
bool chat_learn(Chat *chat, const char *text);

/* Удаляет всё выученное, оставляя встроенный набор. */
void chat_forget(Chat *chat);

/* 0..1, симметричная. Композитная метрика. */
float chat_similarity(const char *a, const char *b);

/* Выбирает ответ. Возвращает указатель на внутреннюю строку,
 * валидную до следующего изменения памяти. */
const char *chat_reply(Chat *chat, const char *input);

/* Принудительная генерация цепью Маркова.
 * temperature: 0.7 — сдержанно, 1.0 — обычно, 1.6 — сумбурно. */
const char *chat_generate(Chat *chat, const char *prompt, double temperature);

/* Случайная фраза из всей памяти (для приветствия). */
const char *chat_random(Chat *chat);

/* Диагностика для /stats. */
size_t chat_markov_states(const Chat *chat);
size_t chat_markov_vocab(const Chat *chat);
double chat_perplexity(const Chat *chat, const char *text);

/* Формат: UTF-8, одна фраза на строку, '#' — комментарий.
 * Сохраняются только выученные фразы. */
bool chat_save(const Chat *chat, const char *path);
size_t chat_load(Chat *chat, const char *path);

#endif /* HELLO_CHAT_H */
