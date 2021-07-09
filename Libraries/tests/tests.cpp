#include "tests.h"

int tests_run = 0;
int tests_failed = 0;

int main() {
    FakeSerialTests();
    FixedSizeStringTests();
    LookupTableTests();

    mu_tests_finished();
    return 0;
}
