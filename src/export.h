/*
 * export.h — сохранение диалога в Markdown.
 *
 * Работает с массивом строк, а не с View: так экспорт тестируется
 * без SDL и переиспользуется вариантами.
 */
#ifndef HELLO_EXPORT_H
#define HELLO_EXPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "viewkind.h"

typedef struct {
    const char *text;
    ViewKind kind;
    uint32_t at_ms; /* миллисекунды от старта сессии */
} ExportLine;

/* Записывает диалог в файл.
 * title — заголовок документа, meta — необязательный блок сведений
 * (статистика сессии), может быть NULL. */
bool export_markdown(const ExportLine *lines, size_t count, const char *path,
                     const char *title, const char *meta);

/* То же, но в строку — для тестов и для показа в интерфейсе. */
bool export_markdown_string(const ExportLine *lines, size_t count,
                            const char *title, const char *meta, char **out);

#endif /* HELLO_EXPORT_H */
