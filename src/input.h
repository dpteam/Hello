/*
 * input.h — строка ввода с UTF-8-курсором и историей.
 * Чистая логика без SDL: покрывается юнит-тестами.
 */
#ifndef HELLO_INPUT_H
#define HELLO_INPUT_H

#include <stdbool.h>
#include <stddef.h>

#define INPUT_MAX_BYTES 480
#define INPUT_MAX_HISTORY 64

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
    size_t cursor;

    char **history;
    size_t history_count;
    size_t history_cap;
    int history_pos; /* -1 — редактируем новую строку */
    char *draft;
} Input;

void input_init(Input *in);
void input_free(Input *in);

void input_clear(Input *in);

bool input_insert(Input *in, const char *text);
bool input_backspace(Input *in);
bool input_delete(Input *in);

bool input_move_left(Input *in);
bool input_move_right(Input *in);
void input_home(Input *in);
void input_end(Input *in);

/* Возвращает false для пустой строки. */
bool input_remember(Input *in, const char *text);

/* true, если строка изменилась. */
bool input_history_prev(Input *in);
bool input_history_next(Input *in);

const char *input_text(const Input *in);
size_t input_cursor(const Input *in);
size_t input_history_count(const Input *in);
const char *input_history_at(const Input *in, size_t index);

#endif /* HELLO_INPUT_H */
