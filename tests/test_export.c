#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"

static void test_empty(void)
{
    char *out = NULL;
    CHECK(export_markdown_string(NULL, 0, "Заголовок", NULL, &out));
    CHECK(out != NULL);
    CHECK(strstr(out, "# Заголовок") != NULL);
    free(out);
}

static void test_dialogue(void)
{
    ExportLine lines[3] = {
        {"привет", VIEW_USER, 0},
        {"Здарова, Привет ёпта", VIEW_BOT, 1500},
        {"Тема: amber", VIEW_SYSTEM, 61000},
    };

    char *out = NULL;
    CHECK(export_markdown_string(lines, 3, "Диалог", NULL, &out));
    CHECK(out != NULL);

    /* Заголовок. */
    CHECK(strstr(out, "# Диалог") != NULL);

    /* Реплики с метками времени и автором. */
    CHECK(strstr(out, "**[00:00] Вы:** привет") != NULL);
    CHECK(strstr(out, "**[00:01] Hello:** Здарова") != NULL);

    /* Системная строка — цитатой, без автора, но с временем. */
    CHECK(strstr(out, "> [01:01] Тема: amber") != NULL);

    /* Минуты считаются правильно. */
    CHECK(strstr(out, "[01:01]") != NULL);

    free(out);
}

static void test_meta(void)
{
    ExportLine line = {"текст", VIEW_USER, 0};
    char *out = NULL;

    CHECK(export_markdown_string(&line, 1, "T", "Фраз: 23", &out));
    CHECK(strstr(out, "Фраз: 23") != NULL);
    free(out);

    /* Пустая meta не добавляет лишнего. */
    CHECK(export_markdown_string(&line, 1, "T", "", &out));
    CHECK(strstr(out, "Фраз") == NULL);
    free(out);
}

static void test_escaping(void)
{
    ExportLine line = {"*звёзды* и _подчёркивания_ и `код`", VIEW_USER, 0};
    char *out = NULL;

    CHECK(export_markdown_string(&line, 1, "T", NULL, &out));
    CHECK(strstr(out, "\\*звёзды\\*") != NULL);
    CHECK(strstr(out, "\\_подчёркивания\\_") != NULL);
    CHECK(strstr(out, "\\`код\\`") != NULL);
    free(out);
}

static void test_null_text(void)
{
    ExportLine lines[2] = {
        {NULL, VIEW_USER, 0},
        {"нормально", VIEW_BOT, 0},
    };

    char *out = NULL;
    CHECK(export_markdown_string(lines, 2, "T", NULL, &out));
    CHECK(strstr(out, "нормально") != NULL);
    free(out);
}

static void test_file(void)
{
    const char *path = "test_export.tmp";
    ExportLine line = {"строка", VIEW_BOT, 1000};

    CHECK(export_markdown(&line, 1, path, "Файл", NULL));

    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    if (f != NULL) {
        char buf[512] = {0};
        size_t n = fread(buf, 1, sizeof(buf) - 1, f);
        buf[n] = '\0';
        fclose(f);
        CHECK(strstr(buf, "# Файл") != NULL);
        CHECK(strstr(buf, "строка") != NULL);
    }
    remove(path);

    /* Запись в несуществующий каталог — false, без падения. */
    CHECK(!export_markdown(&line, 1, "нет/такой/папки/x.md", "T", NULL));
    CHECK(!export_markdown(NULL, 0, NULL, "T", NULL));
}

void test_export(void)
{
    test_empty();
    test_dialogue();
    test_meta();
    test_escaping();
    test_null_text();
    test_file();
}
