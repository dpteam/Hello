#include "view.h"

#include <stdlib.h>
#include <string.h>

#include "render.h"
#include "utf8.h"

#define VIEW_MEASURE_MAX 2048

SDL_Color view_color(const Theme *theme, ViewKind kind)
{
    if (theme == NULL)
        return (SDL_Color){255, 255, 255, 255};

    switch (kind) {
    case VIEW_USER:
        return theme->user;
    case VIEW_BOT:
        return theme->bot;
    case VIEW_WARNING:
        return theme->warning;
    case VIEW_ERROR:
        return theme->error;
    case VIEW_HINT:
        return theme->hint;
    case VIEW_SYSTEM:
    default:
        return theme->system;
    }
}

static void free_row(ViewRow *row)
{
    if (row->tex != NULL)
        SDL_DestroyTexture(row->tex);
    row->tex = NULL;
}

static bool reserve_rows(View *view, size_t extra)
{
    if (view->row_count + extra <= view->row_cap)
        return true;

    size_t cap = view->row_cap ? view->row_cap : 64;
    while (cap < view->row_count + extra)
        cap *= 2;

    ViewRow *grown = (ViewRow *)realloc(view->rows, cap * sizeof(ViewRow));
    if (grown == NULL)
        return false;

    view->rows = grown;
    view->row_cap = cap;
    return true;
}

static bool push_row(View *view, const char *text, size_t len, ViewKind kind,
                     TTF_Font *font, SDL_Renderer *ren, const Theme *theme,
                     size_t line_index, size_t offset)
{
    if (!reserve_rows(view, 1))
        return false;

    char stack_buf[VIEW_MEASURE_MAX];
    char *copy = stack_buf;

    if (len + 1 > sizeof(stack_buf)) {
        copy = (char *)malloc(len + 1);
        if (copy == NULL)
            return false;
    }

    memcpy(copy, text, len);
    copy[len] = '\0';

    ViewRow *row = &view->rows[view->row_count];
    row->tex = render_text(ren, font, copy, view_color(theme, kind));
    row->w = 0;
    row->h = 0;
    row->kind = kind;
    row->line_index = line_index;
    row->offset = offset;
    row->length = len;

    if (row->tex != NULL) {
        float fw = 0.0f, fh = 0.0f;
        SDL_GetTextureSize(row->tex, &fw, &fh);
        row->w = (int)fw;
        row->h = (int)fh;
    }

    if (copy != stack_buf)
        free(copy);

    ++view->row_count;
    return true;
}

static bool wrap_line(View *view, const char *text, size_t len, ViewKind kind,
                      TTF_Font *font, SDL_Renderer *ren, const Theme *theme,
                      int max_width, size_t line_index, size_t base_offset)
{
    if (max_width <= 0) {
        return push_row(view, text, len, kind, font, ren, theme, line_index,
                        base_offset);
    }

    char measure[VIEW_MEASURE_MAX];
    size_t pos = 0;

    while (pos < len) {
        size_t row_start = pos;
        size_t best = pos;
        size_t last_space = (size_t)-1;
        size_t cursor = pos;

        while (cursor < len) {
            size_t next = utf8_next(text, cursor);
            if (next > len)
                next = len;

            size_t probe = next - row_start;
            if (probe >= sizeof(measure))
                probe = sizeof(measure) - 1;
            memcpy(measure, text + row_start, probe);
            measure[probe] = '\0';

            int w = 0;
            render_measure(font, measure, &w, NULL);

            if (w > max_width && next > row_start)
                break;

            if (text[cursor] == ' ' || text[cursor] == '\t')
                last_space = cursor;

            cursor = next;
            best = next;
        }

        if (cursor < len) {
            if (last_space != (size_t)-1 && last_space > row_start)
                best = last_space;
            else if (best <= row_start)
                best = utf8_next(text, row_start); /* один символ шире окна */

            if (best <= row_start)
                best = row_start + 1; /* страховка от зацикливания */
        }

        size_t row_end = best;
        while (row_end > row_start &&
               (text[row_end - 1] == ' ' || text[row_end - 1] == '\t'))
            --row_end;

        if (row_end > row_start) {
            if (!push_row(view, text + row_start, row_end - row_start, kind,
                          font, ren, theme, line_index, base_offset + row_start))
                return false;
        }

        pos = best;
        while (pos < len && (text[pos] == ' ' || text[pos] == '\t'))
            ++pos;
    }

    return true;
}

static bool add_line(View *view, const char *text, ViewKind kind,
                     TTF_Font *font, SDL_Renderer *ren, const Theme *theme,
                     int wrap_width)
{
    if (view->line_count == view->line_cap) {
        size_t cap = view->line_cap ? view->line_cap * 2 : 32;
        ViewLine *grown = (ViewLine *)realloc(view->lines, cap * sizeof(ViewLine));
        if (grown == NULL)
            return false;
        view->lines = grown;
        view->line_cap = cap;
    }

    char *copy = utf8_dup(text ? text : "");
    if (copy == NULL)
        return false;

    size_t index = view->line_count;
    view->lines[index].text = copy;
    view->lines[index].kind = kind;
    /* SDL_GetTicks в SDL3 возвращает Uint64 — храним младшие 32 бита. */
    view->lines[index].at_ms = (uint32_t)SDL_GetTicks();
    ++view->line_count;

    size_t len = strlen(copy);
    size_t pos = 0;

    while (pos <= len) {
        size_t end = pos;
        while (end < len && copy[end] != '\n')
            ++end;

        if (end > pos || pos == 0) {
            if (!wrap_line(view, copy + pos, end - pos, kind, font, ren, theme,
                           wrap_width, index, pos))
                return false;
        }

        if (end >= len)
            break;
        pos = end + 1;
    }

    return true;
}

static void trim_lines(View *view)
{
    if (view->max_lines == 0 || view->line_count <= view->max_lines)
        return;

    size_t drop = view->line_count - view->max_lines;
    for (size_t i = 0; i < drop; ++i)
        free(view->lines[i].text);

    memmove(view->lines, view->lines + drop,
            (view->line_count - drop) * sizeof(ViewLine));
    view->line_count -= drop;
}

static void rebuild_rows(View *view, TTF_Font *font, SDL_Renderer *ren,
                         const Theme *theme, int wrap_width)
{
    for (size_t i = 0; i < view->row_count; ++i)
        free_row(&view->rows[i]);
    view->row_count = 0;
    view->scroll = 0;

    for (size_t i = 0; i < view->line_count; ++i) {
        ViewLine *line = &view->lines[i];
        size_t len = strlen(line->text);
        size_t pos = 0;

        while (pos <= len) {
            size_t end = pos;
            while (end < len && line->text[end] != '\n')
                ++end;

            if (end > pos || pos == 0)
                wrap_line(view, line->text + pos, end - pos, line->kind, font,
                          ren, theme, wrap_width, i, pos);

            if (end >= len)
                break;
            pos = end + 1;
        }
    }
}

void view_init(View *view, size_t max_lines)
{
    if (view == NULL)
        return;
    memset(view, 0, sizeof(*view));
    view->max_lines = max_lines;
}

void view_destroy(View *view)
{
    if (view == NULL)
        return;

    view_clear(view);
    free(view->lines);
    free(view->rows);
    memset(view, 0, sizeof(*view));
}

void view_clear(View *view)
{
    if (view == NULL)
        return;

    for (size_t i = 0; i < view->line_count; ++i)
        free(view->lines[i].text);
    view->line_count = 0;

    for (size_t i = 0; i < view->row_count; ++i)
        free_row(&view->rows[i]);
    view->row_count = 0;
    view->scroll = 0;
}

bool view_add(View *view, const char *text, ViewKind kind,
              TTF_Font *font, SDL_Renderer *ren, const Theme *theme,
              int wrap_width)
{
    if (view == NULL || text == NULL)
        return false;

    view->wrap_width = wrap_width;

    if (!add_line(view, text, kind, font, ren, theme, wrap_width))
        return false;

    size_t before = view->line_count;
    trim_lines(view);
    if (before != view->line_count)
        rebuild_rows(view, font, ren, theme, wrap_width);

    return true;
}

void view_relayout(View *view, TTF_Font *font, SDL_Renderer *ren,
                   const Theme *theme, int wrap_width)
{
    if (view == NULL)
        return;

    view->wrap_width = wrap_width;
    rebuild_rows(view, font, ren, theme, wrap_width);
}

int view_max_scroll(const View *view, int visible_rows)
{
    if (view == NULL || visible_rows <= 0)
        return 0;
    if ((int)view->row_count <= visible_rows)
        return 0;
    return (int)view->row_count - visible_rows;
}

void view_scroll(View *view, int rows, int visible_rows)
{
    if (view == NULL)
        return;

    int max_scroll = view_max_scroll(view, visible_rows);
    int next = view->scroll + rows;
    if (next < 0)
        next = 0;
    if (next > max_scroll)
        next = max_scroll;
    view->scroll = next;
}

int view_scroll_offset(const View *view)
{
    return view ? view->scroll : 0;
}

int view_visible_start(const View *view, int visible_rows)
{
    if (view == NULL || visible_rows <= 0)
        return 0;

    int total = (int)view->row_count;
    int start = total - visible_rows - view->scroll;
    if (start < 0)
        start = 0;
    return start;
}

void view_draw(const View *view, SDL_Renderer *ren, int x, int y_top,
               int line_h, int visible_rows)
{
    if (view == NULL || ren == NULL || visible_rows <= 0)
        return;

    int total = (int)view->row_count;
    int start = view_visible_start(view, visible_rows);

    for (int i = 0; i < visible_rows; ++i) {
        int index = start + i;
        if (index >= total)
            break;

        const ViewRow *row = &view->rows[index];
        if (row->tex == NULL)
            continue;

        int y = y_top + i * line_h + (line_h - row->h) / 2;
        SDL_FRect dst = {(float)x, (float)y, (float)row->w, (float)row->h};
        SDL_RenderTexture(ren, row->tex, NULL, &dst);
    }
}

size_t view_line_count(const View *view)
{
    return view ? view->line_count : 0;
}

const ViewLine *view_line_at(const View *view, size_t index)
{
    if (view == NULL || index >= view->line_count)
        return NULL;
    return &view->lines[index];
}

int view_row_at(const View *view, int mouse_y, int area_y, int line_h,
                int visible_rows)
{
    if (view == NULL || line_h <= 0 || visible_rows <= 0)
        return -1;

    int rel = mouse_y - area_y;
    if (rel < 0)
        return -1;

    int slot = rel / line_h;
    if (slot < 0 || slot >= visible_rows)
        return -1;

    int total = (int)view->row_count;
    int start = view_visible_start(view, visible_rows);
    int index = start + slot;
    if (index < 0 || index >= total)
        return -1;

    return index;
}

size_t view_row_text(const View *view, int row, char *out, size_t cap)
{
    if (view == NULL || out == NULL || cap == 0)
        return 0;

    if (row < 0 || row >= (int)view->row_count) {
        out[0] = '\0';
        return 0;
    }

    const ViewRow *r = &view->rows[row];
    if (r->line_index >= view->line_count) {
        out[0] = '\0';
        return 0;
    }

    const char *src = view->lines[r->line_index].text;
    size_t src_len = strlen(src);
    if (r->offset >= src_len) {
        out[0] = '\0';
        return 0;
    }

    size_t len = r->length;
    if (r->offset + len > src_len)
        len = src_len - r->offset;
    if (len >= cap)
        len = cap - 1;

    memcpy(out, src + r->offset, len);
    out[len] = '\0';
    return len;
}

size_t view_rows_text(const View *view, int from, int to, char *out,
                      size_t cap)
{
    if (view == NULL || out == NULL || cap == 0)
        return 0;

    out[0] = '\0';

    if (from > to) {
        int swap = from;
        from = to;
        to = swap;
    }

    size_t used = 0;
    for (int row = from; row <= to; ++row) {
        char line[VIEW_MEASURE_MAX];
        size_t len = view_row_text(view, row, line, sizeof(line));
        if (len == 0 && row != from)
            continue;

        if (used + len + 2 > cap)
            break;

        if (used > 0)
            out[used++] = '\n';

        memcpy(out + used, line, len);
        used += len;
        out[used] = '\0';
    }

    return used;
}
