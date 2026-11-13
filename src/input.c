#include "input.h"

#include <stdlib.h>
#include <string.h>

#include "utf8.h"

static bool ensure_capacity(Input *in, size_t need)
{
    if (need <= in->cap)
        return true;

    size_t cap = in->cap ? in->cap : 64;
    while (cap < need)
        cap *= 2;

    char *grown = (char *)realloc(in->buf, cap);
    if (grown == NULL)
        return false;

    in->buf = grown;
    in->cap = cap;
    return true;
}

static void replace_with(Input *in, const char *text)
{
    size_t len = strlen(text);
    if (len > INPUT_MAX_BYTES)
        len = utf8_clip(text, INPUT_MAX_BYTES);

    if (!ensure_capacity(in, len + 1))
        return;

    memcpy(in->buf, text, len);
    in->buf[len] = '\0';
    in->len = len;
    in->cursor = len;
}

static void save_draft(Input *in)
{
    free(in->draft);
    in->draft = utf8_dup(in->buf ? in->buf : "");
}

static void restore_draft(Input *in)
{
    if (in->draft != NULL) {
        replace_with(in, in->draft);
        free(in->draft);
        in->draft = NULL;
    }
}

void input_init(Input *in)
{
    if (in == NULL)
        return;

    memset(in, 0, sizeof(*in));
    in->history_pos = -1;
    ensure_capacity(in, 64);
    if (in->buf != NULL)
        in->buf[0] = '\0';
}

void input_free(Input *in)
{
    if (in == NULL)
        return;

    free(in->buf);
    free(in->draft);
    for (size_t i = 0; i < in->history_count; ++i)
        free(in->history[i]);
    free(in->history);
    memset(in, 0, sizeof(*in));
    in->history_pos = -1;
}

void input_clear(Input *in)
{
    if (in == NULL)
        return;

    if (in->buf != NULL)
        in->buf[0] = '\0';
    in->len = 0;
    in->cursor = 0;
    free(in->draft);
    in->draft = NULL;
    in->history_pos = -1;
}

bool input_insert(Input *in, const char *text)
{
    if (in == NULL || text == NULL || text[0] == '\0')
        return false;

    size_t add = strlen(text);
    if (in->len + add > INPUT_MAX_BYTES)
        add = utf8_clip(text, INPUT_MAX_BYTES - in->len);
    if (add == 0)
        return false;

    if (!ensure_capacity(in, in->len + add + 1))
        return false;

    memmove(in->buf + in->cursor + add, in->buf + in->cursor,
            in->len - in->cursor + 1);
    memcpy(in->buf + in->cursor, text, add);
    in->len += add;
    in->cursor += add;
    return true;
}

bool input_backspace(Input *in)
{
    if (in == NULL || in->cursor == 0)
        return false;

    size_t from = utf8_prev(in->buf, in->cursor);
    size_t removed = in->cursor - from;

    memmove(in->buf + from, in->buf + in->cursor, in->len - in->cursor + 1);
    in->len -= removed;
    in->cursor = from;
    return true;
}

bool input_delete(Input *in)
{
    if (in == NULL || in->cursor >= in->len)
        return false;

    size_t to = utf8_next(in->buf, in->cursor);
    size_t removed = to - in->cursor;

    memmove(in->buf + in->cursor, in->buf + to, in->len - to + 1);
    in->len -= removed;
    return true;
}

bool input_move_left(Input *in)
{
    if (in == NULL || in->cursor == 0)
        return false;
    in->cursor = utf8_prev(in->buf, in->cursor);
    return true;
}

bool input_move_right(Input *in)
{
    if (in == NULL || in->cursor >= in->len)
        return false;
    in->cursor = utf8_next(in->buf, in->cursor);
    return true;
}

void input_home(Input *in)
{
    if (in != NULL)
        in->cursor = 0;
}

void input_end(Input *in)
{
    if (in != NULL)
        in->cursor = in->len;
}

bool input_remember(Input *in, const char *text)
{
    if (in == NULL || text == NULL)
        return false;

    const char *start = text;
    while (*start == ' ' || *start == '\t')
        ++start;

    size_t len = strlen(start);
    while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t'))
        --len;
    if (len == 0)
        return false;

    if (in->history_count > 0) {
        const char *last = in->history[in->history_count - 1];
        if (strlen(last) == len && memcmp(last, start, len) == 0)
            return false;
    }

    if (in->history_count == in->history_cap) {
        size_t cap = in->history_cap ? in->history_cap * 2 : 16;
        char **grown = (char **)realloc(in->history, cap * sizeof(char *));
        if (grown == NULL)
            return false;
        in->history = grown;
        in->history_cap = cap;
    }

    char *copy = utf8_dup_range(start, len);
    if (copy == NULL)
        return false;

    if (in->history_count == INPUT_MAX_HISTORY) {
        free(in->history[0]);
        memmove(in->history, in->history + 1,
                (in->history_count - 1) * sizeof(char *));
        --in->history_count;
    }

    in->history[in->history_count++] = copy;
    return true;
}

bool input_history_prev(Input *in)
{
    if (in == NULL || in->history_count == 0)
        return false;

    if (in->history_pos == -1) {
        save_draft(in);
        in->history_pos = (int)in->history_count - 1;
    } else if (in->history_pos > 0) {
        --in->history_pos;
    } else {
        return false;
    }

    replace_with(in, in->history[in->history_pos]);
    return true;
}

bool input_history_next(Input *in)
{
    if (in == NULL || in->history_pos == -1)
        return false;

    if (in->history_pos + 1 < (int)in->history_count) {
        ++in->history_pos;
        replace_with(in, in->history[in->history_pos]);
        return true;
    }

    in->history_pos = -1;
    restore_draft(in);
    return true;
}

const char *input_text(const Input *in)
{
    return (in && in->buf) ? in->buf : "";
}

size_t input_cursor(const Input *in)
{
    return in ? in->cursor : 0;
}

size_t input_history_count(const Input *in)
{
    return in ? in->history_count : 0;
}

const char *input_history_at(const Input *in, size_t index)
{
    if (in == NULL || index >= in->history_count)
        return NULL;
    return in->history[index];
}
