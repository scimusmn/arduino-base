#ifndef MINUNIT_H
#define MINUNIT_H

#include <stdio.h>
#include "colors.h"

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

/* minunit testing macros from /www.jera.com/techinfo/jtns/jtn002.html */
#define mu_assert(test, message) do { if (!(test)) return message " [line " STR(__LINE__) "]"; } while (0)
#define mu_assert_equal(a, b) mu_assert(a == b, "'" #a "' is not equal to '" #b "'")
#define mu_assert_unequal(a, b) mu_assert(a != b, "'" #a "' is equal to '" #b "'")
#define mu_assert_streq(a, b) mu_assert(strcmp(a, b) == 0, "'" #a "' is not equal to '" #b "'")
#define mu_run_test(name, test) do {                            \
	const char *message = test();					\
	tests_run++;                                            \
	if (message) {							\
	    printf(RED "   test '%s' failed: %s\n" RESET, name, message); \
	    tests_failed++;						\
	}                                                       \
    } while (0)
#define mu_tests_finished() do { \
    if (tests_failed > 0) { printf("ran %d tests, " RED "%d" RESET " failed\n", tests_run, tests_failed); } \
    else { printf("ran %d tests, " GREEN "all succeeded\n" RESET, tests_run); } \
    } while(0)

#define mu_test const char *

extern int tests_run, tests_failed;

#endif
