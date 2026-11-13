#include "test.h"

#include <stdio.h>
#include <string.h>

#include "config.h"

static void test_defaults(void)
{
    Config cfg;
    config_defaults(&cfg);

    CHECK(cfg.width >= 320);
    CHECK(cfg.height >= 240);
    CHECK(cfg.font_size > 0);
    CHECK(cfg.max_lines > 0);
    CHECK_STR_EQ(cfg.theme, "green");
    CHECK(strstr(cfg.font_path, "DejaVuSansMono-Bold.ttf") != NULL);

    /* NULL не должен ронять. */
    config_defaults(NULL);
}

static void test_roundtrip(void)
{
    const char *path = "test_config_roundtrip.tmp";

    Config cfg;
    config_defaults(&cfg);
    cfg.width = 1024;
    cfg.height = 700;
    cfg.font_size = 18;
    cfg.max_lines = 500;
    cfg.fps_limit = 60;
    cfg.vsync = false;
    snprintf(cfg.theme, sizeof(cfg.theme), "amber");
    snprintf(cfg.memory_path, sizeof(cfg.memory_path), "custom.memory");

    CHECK(config_save(&cfg, path));

    Config loaded;
    config_defaults(&loaded);
    CHECK(config_load(&loaded, path));

    CHECK_INT_EQ(loaded.width, 1024);
    CHECK_INT_EQ(loaded.height, 700);
    CHECK_INT_EQ(loaded.font_size, 18);
    CHECK_INT_EQ(loaded.max_lines, 500);
    CHECK_INT_EQ(loaded.fps_limit, 60);
    CHECK(!loaded.vsync);
    CHECK_STR_EQ(loaded.theme, "amber");
    CHECK_STR_EQ(loaded.memory_path, "custom.memory");

    remove(path);
}

static void test_missing_file(void)
{
    Config cfg;
    config_defaults(&cfg);
    CHECK(!config_load(&cfg, "no_such_config_12345.cfg"));
    /* Дефолты не пострадали. */
    CHECK_INT_EQ(cfg.width, 800);
}

static void test_parse_quirks(void)
{
    const char *path = "test_config_quirks.tmp";

    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        fputs("# комментарий\n", f);
        fputs("; ещё комментарий\n", f);
        fputs("[section]\n", f);
        fputs("  WIDTH   =   1280  \n", f);   /* регистр и пробелы */
        fputs("height = \"600\"\n", f);        /* кавычки */
        fputs("theme = 'ice'\n", f);           /* одинарные кавычки */
        fputs("vsync = no\n", f);
        fputs("fps_limit = abc\n", f);         /* мусор — игнор */
        fputs("unknown_key = 1\n", f);         /* игнор */
        fputs("плохая строка без равно\n", f); /* игнор */
        fputs("font_size = 999\n", f);         /* клампится */
        fclose(f);
    }

    Config cfg;
    config_defaults(&cfg);
    CHECK(config_load(&cfg, path));

    CHECK_INT_EQ(cfg.width, 1280);
    CHECK_INT_EQ(cfg.height, 600);
    CHECK_STR_EQ(cfg.theme, "ice");
    CHECK(!cfg.vsync);
    CHECK_INT_EQ(cfg.fps_limit, 0);
    CHECK_INT_EQ(cfg.font_size, 96);

    remove(path);
}

static void test_save_failure(void)
{
    Config cfg;
    config_defaults(&cfg);
    CHECK(!config_save(&cfg, ""));
    CHECK(!config_save(NULL, "x.cfg"));
    CHECK(!config_load(NULL, "x.cfg"));
}

void test_config(void)
{
    test_defaults();
    test_roundtrip();
    test_missing_file();
    test_parse_quirks();
    test_save_failure();
}
