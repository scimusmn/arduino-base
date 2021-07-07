#include "tests.h"
#include "../FixedSizeString.h"

mu_test fss_create() {
    smm::FixedSizeString<32> string32;
    mu_assert_equal(string32.maxSize(), 32);
    return 0;
}


mu_test fss_initialize() {
    smm::FixedSizeString<32> string("hello, world!");
    mu_assert_streq(string.c_str(), "hello, world!");
    mu_assert_equal(string.size(), 13);
    return 0;
}


mu_test fss_initialize_overflow() {
    smm::FixedSizeString<5> string("hello");
    mu_assert_streq(string.c_str(), "hell");
    mu_assert_equal(string.size(), 4);
    return 0;
}


mu_test fss_append() {
    smm::FixedSizeString<32> string("hello, ");
    string.append("world!");
    mu_assert_streq(string.c_str(), "hello, world!");
    mu_assert_equal(string.size(), 13);
    return 0;
}

mu_test fss_append_overflow() {
    smm::FixedSizeString<10> string("hello, ");
    string.append("world!");
    mu_assert_streq(string.c_str(), "hello, wo");
    mu_assert_equal(string.size(), 9);
    return 0;
}

mu_test fss_append_char() {
    smm::FixedSizeString<32> string("hell");
    string.append('o');
    mu_assert_streq(string.c_str(), "hello");
    mu_assert_equal(string.size(), 5);
    return 0;
}

mu_test fss_equality() {
    smm::FixedSizeString<32> string1("hello");
    smm::FixedSizeString<32> string2("hello");
    mu_assert_equal(string1, string2);
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FixedSizeStringTests() {
    printf("running tests for FixedSizeString\n");
    mu_run_test("create and allocate space for string", fss_create);
    mu_run_test("initialize string value", fss_initialize);
    mu_run_test("string initialization overflow", fss_initialize_overflow);
    
    mu_run_test("append to string", fss_append);
    mu_run_test("append character to string", fss_append_char);
    mu_run_test("string equality operator", fss_equality);
}
