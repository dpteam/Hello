#include "export.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

/* Экранирует символы, которые ломают Markdown внутри абзаца. */
static bool append_escaped(char **buf, size_t *len, size_t *cap,
                           const char *text)
{
    const char *run = text;

    for (const char *p = text;; ++p) {
        bool special = (*p == '\\' || *p == '*' || *p == '_' || *p == '`' ||
                        *p == '<' || *p == '>' || *p == '[' || *p == ']' ||
                        *p == '|');
        if (!special && *p != '\0')
            continue;

        if (p > run) {
            if (!utf8_buf_append(buf, len, cap, run, (size_t)(p - run)))
                return false;
        }
        if (*p == '\0')
            break;

        char escaped[2] = {'\\', *p};
        if (!utf8_buf_append(buf, len, cap, escaped, 2))
            return false;
        run = p + 1;
    }
    return true;
}

static bool append_raw(char **buf, size_t *len, size_t *cap, const char *text)
{
    return utf8_buf_append(buf, len, cap, text, strlen(text));
}

/* «00:12» из миллисекунд. */
static void format_time(uint32_t at_ms, char *out, size_t cap)
{
    uint32_t total = at_ms / 1000u;
    snprintf(out, cap, "%02u:%02u", total / 60u, total % 60u);
}

static bool append_line(char **buf, size_t *len, size_t *cap,
                        const ExportLine *line)
{
    char stamp[16];
    format_time(line->at_ms, stamp, sizeof(stamp));

    const char *author = view_kind_author(line->kind);

    if (author != NULL) {
        /* Реплика: **[00:12] Вы:** текст */
        if (!append_raw(buf, len, cap, "**[") ||
            !append_raw(buf, len, cap, stamp) ||
            !append_raw(buf, len, cap, "] ") ||
            !append_raw(buf, len, cap, author) ||
            !append_raw(buf, len, cap, ":** "))
            return false;
    } else {
        /* Системная строка: > [00:12] текст */
        if (!append_raw(buf, len, cap, "> [") ||
            !append_raw(buf, len, cap, stamp) ||
            !append_raw(buf, len, cap, "] "))
            return false;
    }

    if (!append_escaped(buf, len, cap, line->text))
        return false;

    return append_raw(buf, len, cap, "\n\n");
}

bool export_markdown_string(const ExportLine *lines, size_t count,
                            const char *title, const char *meta, char **out)
{
    if (out == NULL)
        return false;

    *out = NULL;

    char *buf = NULL;
    size_t len = 0;
    size_t cap = 0;

    const char *heading = (title != NULL && title[0] != '\0')
                              ? title
                              : "Codename Hello — диалог";

    if (!append_raw(&buf, &len, &cap, "# ") ||
        !append_escaped(&buf, &len, &cap, heading) ||
        !append_raw(&buf, &len, &cap, "\n\n"))
        goto fail;

    if (meta != NULL && meta[0] != '\0') {
        if (!append_raw(&buf, &len, &cap, meta) ||
            !append_raw(&buf, &len, &cap, "\n\n"))
            goto fail;
    }

    if (count > 0 && !append_raw(&buf, &len, &cap, "---\n\n"))
        goto fail;

    for (size_t i = 0; i < count; ++i) {
        if (lines[i].text == NULL)
            continue;
        if (!append_line(&buf, &len, &cap, &lines[i]))
            goto fail;
    }

    *out = buf;
    return true;

fail:
    free(buf);
    return false;
}

bool export_markdown(const ExportLine *lines, size_t count, const char *path,
                     const char *title, const char *meta)
{
    if (path == NULL)
        return false;

    char *text = NULL;
    if (!export_markdown_string(lines, count, title, meta, &text))
        return false;

    FILE *out = fopen(path, "wb");
    if (out == NULL) {
        free(text);
        return false;
    }

    size_t bytes = strlen(text);
    bool ok = fwrite(text, 1, bytes, out) == bytes;
    if (fclose(out) != 0)
        ok = false;

    free(text);
    return ok;
}
