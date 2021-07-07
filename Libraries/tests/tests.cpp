#include "tests.h"

int tests_run = 0;
int tests_failed = 0;

int main() {
    FixedSizeStringTests();

    mu_tests_finished();
    return 0;
}
