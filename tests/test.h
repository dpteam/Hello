/*
 * test.h — крошечный тестовый каркас без внешних зависимостей.
 */
#ifndef HELLO_TEST_H
#define HELLO_TEST_H

#include <stdio.h>
#include <string.h>

extern int g_checks;
extern int g_failures;
extern const char *g_current_test;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++g_checks;                                                          \
        if (!(cond)) {                                                       \
            ++g_failures;                                                    \
            fprintf(stderr, "  FAIL %s:%d [%s] %s\n", __FILE__, __LINE__,    \
                    g_current_test, #cond);                                  \
        }                                                                    \
    } while (0)

#define CHECK_STR_EQ(actual, expected)                                       \
    do {                                                                     \
        const char *a_ = (actual);                                           \
        const char *e_ = (expected);                                         \
        ++g_checks;                                                          \
        if (a_ == NULL || e_ == NULL || strcmp(a_, e_) != 0) {                \
            ++g_failures;                                                    \
            fprintf(stderr, "  FAIL %s:%d [%s] \"%s\" != \"%s\"\n", __FILE__, \
                    __LINE__, g_current_test, a_ ? a_ : "(null)",            \
                    e_ ? e_ : "(null)");                                     \
        }                                                                    \
    } while (0)

#define CHECK_INT_EQ(actual, expected)                                       \
    do {                                                                     \
        long a_ = (long)(actual);                                            \
        long e_ = (long)(expected);                                          \
        ++g_checks;                                                          \
        if (a_ != e_) {                                                      \
            ++g_failures;                                                    \
            fprintf(stderr, "  FAIL %s:%d [%s] %ld != %ld\n", __FILE__,       \
                    __LINE__, g_current_test, a_, e_);                       \
        }                                                                    \
    } while (0)

#define CHECK_NEAR(actual, expected, eps)                                    \
    do {                                                                     \
        double a_ = (double)(actual);                                        \
        double e_ = (double)(expected);                                      \
        double d_ = a_ - e_;                                                 \
        if (d_ < 0)                                                          \
            d_ = -d_;                                                        \
        ++g_checks;                                                          \
        if (d_ > (eps)) {                                                    \
            ++g_failures;                                                    \
            fprintf(stderr, "  FAIL %s:%d [%s] %f != %f\n", __FILE__,         \
                    __LINE__, g_current_test, a_, e_);                       \
        }                                                                    \
    } while (0)

#define RUN(fn)                                                              \
    do {                                                                     \
        g_current_test = #fn;                                                \
        int before_ = g_failures;                                            \
        fn();                                                                \
        fprintf(stderr, "  %-34s %s\n", #fn,                                 \
                g_failures == before_ ? "ok" : "FAILED");                    \
    } while (0)

void test_utf8(void);
void test_chat(void);
void test_input(void);
void test_config(void);
void test_metrics(void);
void test_markov(void);
void test_export(void);
void test_pet(void);
void test_viewkind(void);
void test_theme(void);

#endif /* HELLO_TEST_H */
