#include "test.h"

int g_checks = 0;
int g_failures = 0;
const char *g_current_test = "";

int main(void)
{
    fprintf(stderr, "Codename Hello — тесты\n");

    RUN(test_utf8);
    RUN(test_chat);
    RUN(test_input);
    RUN(test_config);
    RUN(test_metrics);
    RUN(test_markov);

    fprintf(stderr, "\nПроверок: %d, провалов: %d\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
