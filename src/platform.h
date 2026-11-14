/*
 * platform.h — общий слой для всех приложений проекта.
 *
 * Здесь всё, что нужно и чат-боту, и вариантам (календарь, куб):
 * консоль на Windows, поиск шрифта, иконка окна, разрешение путей.
 */
#ifndef HELLO_PLATFORM_H
#define HELLO_PLATFORM_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stddef.h>

#include "config.h"

/* Подключает родительскую консоль на Windows, чтобы fprintf работал
 * из GUI-приложения. Возвращает true, если консоль доступна — тогда
 * вызывающий код может писать в stderr вместо модальных диалогов.
 * На других платформах всегда true (stderr и так работает). */
bool hello_attach_console(void);

/* Каталог, из которого запущен бинарник (со слэшем на конце).
 * Пустая строка, если SDL не смог определить. */
const char *hello_base_path(void);

bool hello_file_exists(const char *path);

/* Ищет шрифт: сначала cfg->font_path, потом стандартные места
 * рядом с бинарником и в системе. true — найден. */
bool hello_find_font(const Config *cfg, char *out, size_t cap);

/* Разрешает относительный путь: сначала как есть (от CWD),
 * потом относительно каталога бинарника. */
void hello_resolve_path(const char *relative, char *out, size_t cap);

void hello_set_window_icon(SDL_Window *window);

/* Лог в stderr с переводом строки. */
void hello_log(const char *fmt, ...);

/* Сообщает об ошибке. Если консоль доступна — пишет в stderr,
 * иначе показывает модальный диалог. Возвращает true, если был диалог.
 *
 * Модальное окно в неинтерактивной среде (CI, служба) блокирует
 * процесс навсегда, поэтому выбор не косметический. */
bool hello_show_error(const char *title, const char *message);

/* Открыть окно и рендерер с общими настройками проекта.
 * При screenshot == true окно скрытое и рендерер программный.
 * Возвращает false, если что-то не создалось (лог уже написан). */
bool hello_open_window(const Config *cfg, bool screenshot, const char *title,
                       SDL_Window **out_window, SDL_Renderer **out_renderer);

/* Закрыть окно и рендерер, ничего не делая с NULL. */
void hello_close_window(SDL_Window *window, SDL_Renderer *renderer);

#endif /* HELLO_PLATFORM_H */
