/*
 * view.h — прокручиваемый текст с переносом строк.
 *
 * Логические строки хранятся как UTF-8, визуальные — как готовые
 * текстуры. Пере-вёрстка происходит при добавлении строки и при
 * изменении ширины окна (не каждый кадр).
 */
#ifndef HELLO_VIEW_H
#define HELLO_VIEW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "theme.h"
#include "viewkind.h"

typedef struct {
    char *text;
    ViewKind kind;
    uint32_t at_ms; /* когда добавлена, от старта сессии */
} ViewLine;

typedef struct {
    SDL_Texture *tex;
    int w;
    int h;
    ViewKind kind;

    /* Откуда взялась эта визуальная строка — для копирования текста. */
    size_t line_index;
    size_t offset; /* байтовое смещение внутри lines[line_index].text */
    size_t length;
} ViewRow;

typedef struct {
    ViewLine *lines;
    size_t line_count;
    size_t line_cap;

    ViewRow *rows;
    size_t row_count;
    size_t row_cap;

    size_t max_lines;
    int scroll;      /* сколько визуальных строк прокручено вверх */
    int wrap_width;  /* 0 — без переноса */
} View;

void view_init(View *view, size_t max_lines);
void view_destroy(View *view);
void view_clear(View *view);

/* Добавляет сообщение. Возвращает false, если не хватило памяти. */
bool view_add(View *view, const char *text, ViewKind kind,
              TTF_Font *font, SDL_Renderer *ren, const Theme *theme,
              int wrap_width);

/* Пере-вёрстка всех строк (смена темы, размера шрифта или окна). */
void view_relayout(View *view, TTF_Font *font, SDL_Renderer *ren,
                   const Theme *theme, int wrap_width);

/* Положительное значение — вверх (к старым строкам). */
void view_scroll(View *view, int rows, int visible_rows);

/* 0 — прокрутки нет. */
int view_scroll_offset(const View *view);
int view_max_scroll(const View *view, int visible_rows);

/* Первая видимая визуальная строка. */
int view_visible_start(const View *view, int visible_rows);

/* Рисует область высотой visible_rows строк с нижней привязкой. */
void view_draw(const View *view, SDL_Renderer *ren, int x, int y_top,
               int line_h, int visible_rows);

SDL_Color view_color(const Theme *theme, ViewKind kind);

/* --- Доступ к содержимому ------------------------------------------- */

size_t view_line_count(const View *view);
const ViewLine *view_line_at(const View *view, size_t index);

/* Индекс визуальной строки под курсором. -1 — мимо текста.
 * area_y — верхняя граница области текста. */
int view_row_at(const View *view, int mouse_y, int area_y, int line_h,
                int visible_rows);

/* Текст визуальной строки (без переноса). Возвращает число байт. */
size_t view_row_text(const View *view, int row, char *out, size_t cap);

/* Склеивает диапазон визуальных строк [from..to] через '\n'. */
size_t view_rows_text(const View *view, int from, int to, char *out,
                      size_t cap);

#endif /* HELLO_VIEW_H */
