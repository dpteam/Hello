#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

static void copy_string(char *dst, size_t cap, const char *src)
{
    if (cap == 0)
        return;
    size_t len = strlen(src);
    if (len >= cap)
        len = cap - 1;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

void config_defaults(Config *cfg)
{
    if (cfg == NULL)
        return;

    memset(cfg, 0, sizeof(*cfg));
    cfg->width = 800;
    cfg->height = 520;
    cfg->window_x = CONFIG_WINDOW_POS_ANY;
    cfg->window_y = CONFIG_WINDOW_POS_ANY;
    cfg->save_geometry = true;
    cfg->font_size = 14;
    cfg->max_lines = 2000;
    cfg->fps_limit = 0;
    cfg->vsync = true;

    copy_string(cfg->theme, sizeof(cfg->theme), "green");
    copy_string(cfg->font_path, sizeof(cfg->font_path),
                "assets/fonts/DejaVuSansMono-Bold.ttf");
    copy_string(cfg->memory_path, sizeof(cfg->memory_path), "hello.memory");
    copy_string(cfg->history_path, sizeof(cfg->history_path), "hello.history");
    copy_string(cfg->plugins_path, sizeof(cfg->plugins_path), "plugins");
    cfg->profile[0] = '\0';
}

void config_profile_path(const Config *cfg, const char *base, char *out,
                         size_t cap)
{
    if (cfg == NULL || base == NULL || out == NULL || cap == 0)
        return;

    if (cfg->profile[0] == '\0') {
        snprintf(out, cap, "%s", base);
        return;
    }

    /* «hello.memory» + профиль «work» → «hello.work.memory».
     * Точка в конце имени — разделитель расширения, вставляем перед ней. */
    const char *dot = strrchr(base, '.');
    if (dot == NULL || dot == base) {
        snprintf(out, cap, "%s.%s", base, cfg->profile);
        return;
    }

    size_t stem = (size_t)(dot - base);
    snprintf(out, cap, "%.*s.%s%s", (int)stem, base, cfg->profile, dot);
}

static char *trim(char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
        ++s;

    size_t len = strlen(s);
    while (len > 0) {
        char c = s[len - 1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        s[--len] = '\0';
    }
    return s;
}

static void unquote(char *s)
{
    size_t len = strlen(s);
    if (len >= 2 && ((s[0] == '"' && s[len - 1] == '"') ||
                     (s[0] == '\'' && s[len - 1] == '\''))) {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

static bool parse_int(const char *text, int min, int max, int *out)
{
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text)
        return false;
    while (*end == ' ' || *end == '\t')
        ++end;
    if (*end != '\0')
        return false;
    if (value < min)
        value = min;
    if (value > max)
        value = max;
    *out = (int)value;
    return true;
}

static bool parse_bool(const char *text, bool *out)
{
    if (strcmp(text, "1") == 0 || strcmp(text, "true") == 0 ||
        strcmp(text, "yes") == 0 || strcmp(text, "on") == 0) {
        *out = true;
        return true;
    }
    if (strcmp(text, "0") == 0 || strcmp(text, "false") == 0 ||
        strcmp(text, "no") == 0 || strcmp(text, "off") == 0) {
        *out = false;
        return true;
    }
    return false;
}

static void apply(Config *cfg, const char *key, const char *value)
{
    if (strcmp(key, "width") == 0) {
        parse_int(value, 320, 7680, &cfg->width);
    } else if (strcmp(key, "height") == 0) {
        parse_int(value, 240, 4320, &cfg->height);
    } else if (strcmp(key, "font_size") == 0) {
        parse_int(value, 6, 96, &cfg->font_size);
    } else if (strcmp(key, "max_lines") == 0) {
        parse_int(value, 100, 100000, &cfg->max_lines);
    } else if (strcmp(key, "fps_limit") == 0) {
        parse_int(value, 0, 1000, &cfg->fps_limit);
    } else if (strcmp(key, "vsync") == 0) {
        parse_bool(value, &cfg->vsync);
    } else if (strcmp(key, "theme") == 0) {
        copy_string(cfg->theme, sizeof(cfg->theme), value);
    } else if (strcmp(key, "font") == 0) {
        copy_string(cfg->font_path, sizeof(cfg->font_path), value);
    } else if (strcmp(key, "memory") == 0) {
        copy_string(cfg->memory_path, sizeof(cfg->memory_path), value);
    } else if (strcmp(key, "history") == 0) {
        copy_string(cfg->history_path, sizeof(cfg->history_path), value);
    } else if (strcmp(key, "window_x") == 0) {
        parse_int(value, -32768, 32767, &cfg->window_x);
    } else if (strcmp(key, "window_y") == 0) {
        parse_int(value, -32768, 32767, &cfg->window_y);
    } else if (strcmp(key, "save_geometry") == 0) {
        parse_bool(value, &cfg->save_geometry);
    } else if (strcmp(key, "plugins") == 0) {
        copy_string(cfg->plugins_path, sizeof(cfg->plugins_path), value);
    } else if (strcmp(key, "profile") == 0) {
        copy_string(cfg->profile, sizeof(cfg->profile), value);
    }
    /* Неизвестные ключи молча пропускаем: так проще добавлять новые. */
}

bool config_load(Config *cfg, const char *path)
{
    if (cfg == NULL || path == NULL)
        return false;

    FILE *in = fopen(path, "rb");
    if (in == NULL)
        return false;

    char line[1024];
    while (fgets(line, (int)sizeof(line), in) != NULL) {
        char *text = trim(line);
        if (*text == '\0' || *text == '#' || *text == ';' || *text == '[')
            continue;

        char *eq = strchr(text, '=');
        if (eq == NULL)
            continue;

        *eq = '\0';
        char *key = trim(text);
        char *value = trim(eq + 1);
        unquote(value);

        for (char *p = key; *p; ++p) {
            if (*p >= 'A' && *p <= 'Z')
                *p = (char)(*p - 'A' + 'a');
        }

        apply(cfg, key, value);
    }

    fclose(in);
    return true;
}

bool config_save(const Config *cfg, const char *path)
{
    if (cfg == NULL || path == NULL)
        return false;

    char tmp[CONFIG_PATH_MAX + 8];
    if (strlen(path) + 5 >= sizeof(tmp))
        return false;
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);

    FILE *out = fopen(tmp, "wb");
    if (out == NULL)
        return false;

    fprintf(out, "# Codename Hello — конфигурация\n");
    fprintf(out, "# Пути можно указывать абсолютные или относительно рабочей папки.\n\n");
    fprintf(out, "width = %d\n", cfg->width);
    fprintf(out, "height = %d\n", cfg->height);
    fprintf(out, "window_x = %d\n", cfg->window_x);
    fprintf(out, "window_y = %d\n", cfg->window_y);
    fprintf(out, "save_geometry = %s\n", cfg->save_geometry ? "true" : "false");
    fprintf(out, "font_size = %d\n", cfg->font_size);
    fprintf(out, "max_lines = %d\n", cfg->max_lines);
    fprintf(out, "fps_limit = %d\n", cfg->fps_limit);
    fprintf(out, "vsync = %s\n", cfg->vsync ? "true" : "false");
    fprintf(out, "theme = %s\n", cfg->theme);
    fprintf(out, "font = %s\n", cfg->font_path);
    fprintf(out, "memory = %s\n", cfg->memory_path);
    fprintf(out, "history = %s\n", cfg->history_path);
    fprintf(out, "plugins = %s\n", cfg->plugins_path);
    fprintf(out, "profile = %s\n", cfg->profile);

    bool ok = !ferror(out);
    if (fclose(out) != 0)
        ok = false;

    if (!ok) {
        remove(tmp);
        return false;
    }

    remove(path);
    if (rename(tmp, path) != 0) {
        remove(tmp);
        return false;
    }
    return true;
}
