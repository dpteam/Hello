#include "utf8.h"

#include <stdlib.h>
#include <string.h>

int utf8_sequence_length(unsigned char lead)
{
    if (lead < 0x80)
        return 1;
    if (lead >= 0xC2 && lead <= 0xDF)
        return 2;
    if (lead >= 0xE0 && lead <= 0xEF)
        return 3;
    if (lead >= 0xF0 && lead <= 0xF4)
        return 4;
    return 0;
}

int utf8_decode(const char *s, size_t len, uint32_t *out)
{
    if (s == NULL || len == 0 || s[0] == '\0')
        return 0;

    const unsigned char *p = (const unsigned char *)s;
    int n = utf8_sequence_length(p[0]);
    if (n == 0)
        return -1;
    if (n == 1) {
        if (out)
            *out = p[0];
        return 1;
    }
    if (len < (size_t)n)
        return -1;

    for (int i = 1; i < n; ++i) {
        if ((p[i] & 0xC0) != 0x80)
            return -1;
    }

    uint32_t cp;
    switch (n) {
    case 2:
        cp = ((uint32_t)(p[0] & 0x1F) << 6) | (uint32_t)(p[1] & 0x3F);
        break;
    case 3:
        cp = ((uint32_t)(p[0] & 0x0F) << 12) | ((uint32_t)(p[1] & 0x3F) << 6) |
             (uint32_t)(p[2] & 0x3F);
        if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))
            return -1;
        break;
    default:
        cp = ((uint32_t)(p[0] & 0x07) << 18) | ((uint32_t)(p[1] & 0x3F) << 12) |
             ((uint32_t)(p[2] & 0x3F) << 6) | (uint32_t)(p[3] & 0x3F);
        if (cp < 0x10000 || cp > 0x10FFFF)
            return -1;
        break;
    }

    if (out)
        *out = cp;
    return n;
}

int utf8_encode(uint32_t cp, char out[4])
{
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
        return -1;

    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

size_t utf8_strlen(const char *s)
{
    size_t count = 0;
    size_t pos = 0;
    if (s == NULL)
        return 0;

    while (s[pos] != '\0') {
        int n = utf8_decode(s + pos, strlen(s + pos), NULL);
        pos += (n > 0) ? (size_t)n : 1;
        ++count;
    }
    return count;
}

bool utf8_valid(const char *s)
{
    if (s == NULL)
        return false;

    size_t pos = 0;
    size_t len = strlen(s);
    while (pos < len) {
        int n = utf8_decode(s + pos, len - pos, NULL);
        if (n <= 0)
            return false;
        pos += (size_t)n;
    }
    return true;
}

size_t utf8_prev(const char *s, size_t pos)
{
    if (s == NULL || pos == 0)
        return 0;

    size_t i = pos - 1;
    int guard = 0;
    while (i > 0 && ((unsigned char)s[i] & 0xC0) == 0x80 && guard < 3) {
        --i;
        ++guard;
    }
    return i;
}

size_t utf8_next(const char *s, size_t pos)
{
    if (s == NULL || s[pos] == '\0')
        return pos;

    size_t len = strlen(s + pos);
    int n = utf8_decode(s + pos, len, NULL);
    return pos + ((n > 0) ? (size_t)n : 1);
}

size_t utf8_clip(const char *s, size_t max_bytes)
{
    if (s == NULL)
        return 0;

    size_t len = strlen(s);
    if (len <= max_bytes)
        return len;

    size_t i = max_bytes;
    while (i > 0 && ((unsigned char)s[i] & 0xC0) == 0x80)
        --i;
    return i;
}

uint32_t utf8_casefold(uint32_t cp)
{
    /* ASCII */
    if (cp >= 'A' && cp <= 'Z')
        return cp + 0x20;

    /* Латиница-1: À..Þ, кроме × (0xD7) */
    if (cp >= 0xC0 && cp <= 0xDE && cp != 0xD7)
        return cp + 0x20;

    /* Латиница Extended-A: парные заглавная/строчная */
    if (cp >= 0x100 && cp <= 0x137 && (cp % 2) == 0)
        return cp + 1;
    if (cp >= 0x139 && cp <= 0x148 && (cp % 2) == 1)
        return cp + 1;
    if (cp >= 0x14A && cp <= 0x177 && (cp % 2) == 0)
        return cp + 1;
    if (cp == 0x178)
        return 0xFF;

    /* Кириллица */
    if (cp >= 0x410 && cp <= 0x42F)
        return cp + 0x20;
    if (cp == 0x400)
        return 0x450;
    if (cp == 0x401)
        return 0x451;
    if (cp >= 0x460 && cp <= 0x4FF && (cp % 2) == 0)
        return cp + 1;

    /* Греческий, базовый блок */
    if (cp >= 0x391 && cp <= 0x3AB && cp != 0x3A2)
        return cp + 0x20;

    return cp;
}

bool utf8_is_noise(uint32_t cp)
{
    if (cp <= 0x20 || (cp >= 0x7F && cp <= 0x9F))
        return true;

    /* ASCII-пунктуация */
    if ((cp >= 0x21 && cp <= 0x2F) || (cp >= 0x3A && cp <= 0x40) ||
        (cp >= 0x5B && cp <= 0x60) || (cp >= 0x7B && cp <= 0x7E))
        return true;

    if (cp == 0xA0 || cp == 0xAB || cp == 0xBB || cp == 0xB7)
        return true;

    /* Общая пунктуация и пробелы */
    if (cp >= 0x2000 && cp <= 0x206F)
        return true;

    /* CJK-пунктуация */
    if (cp >= 0x3000 && cp <= 0x303F)
        return true;

    /* Вертикальные/полноширинные формы */
    if (cp >= 0xFE10 && cp <= 0xFE1F)
        return true;
    if (cp >= 0xFF01 && cp <= 0xFF0F)
        return true;
    if (cp >= 0xFF1A && cp <= 0xFF20)
        return true;
    if (cp >= 0xFF3B && cp <= 0xFF40)
        return true;
    if (cp >= 0xFF5B && cp <= 0xFF65)
        return true;

    return false;
}

char *utf8_dup_range(const char *s, size_t n)
{
    if (s == NULL)
        return NULL;

    char *copy = (char *)malloc(n + 1);
    if (copy == NULL)
        return NULL;

    memcpy(copy, s, n);
    copy[n] = '\0';
    return copy;
}

char *utf8_dup(const char *s)
{
    if (s == NULL)
        return NULL;
    return utf8_dup_range(s, strlen(s));
}

bool utf8_buf_append(char **buf, size_t *len, size_t *cap,
                     const char *text, size_t n)
{
    if (buf == NULL || len == NULL || cap == NULL || text == NULL)
        return false;

    if (n == (size_t)-1)
        n = strlen(text);

    if (*len + n + 1 > *cap) {
        size_t want = (*cap == 0) ? 64 : *cap;
        while (want < *len + n + 1)
            want *= 2;

        char *grown = (char *)realloc(*buf, want);
        if (grown == NULL)
            return false;

        *buf = grown;
        *cap = want;
    }

    memcpy(*buf + *len, text, n);
    *len += n;
    (*buf)[*len] = '\0';
    return true;
}
