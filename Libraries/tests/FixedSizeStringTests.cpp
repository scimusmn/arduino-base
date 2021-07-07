#include "tests.h"
#include "../FixedSizeString.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * initialization tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test fss_create() {
    smm::FixedSizeString<32> string32;
    mu_assert_equal(string32.maxLength(), 32);
    return 0;
}


mu_test fss_initialize() {
    smm::FixedSizeString<32> string("hello, world!");
    mu_assert_streq(string.c_str(), "hello, world!");
    mu_assert_equal(string.length(), 13);
    return 0;
}


mu_test fss_initialize_overflow() {
    smm::FixedSizeString<4> string("hello");
    mu_assert_streq(string.c_str(), "hell");
    mu_assert_equal(string.length(), 4);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * appending & overflow tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test fss_append() {
    smm::FixedSizeString<32> string("hello, ");
    mu_assert_streq(string.c_str(), "hello, ");
    mu_assert_equal(string.length(), 7);
    string.append("world!");
    mu_assert_streq(string.c_str(), "hello, world!");
    mu_assert_equal(string.length(), 13);
    return 0;
}


mu_test fss_append_multi() {
    smm::FixedSizeString<32> string;
    mu_assert_streq(string.c_str(), "");
    mu_assert_equal(string.length(), 0);
    string.append("hello");
    mu_assert_streq(string.c_str(), "hello");
    mu_assert_equal(string.length(), 5);
    string.append(", ");
    mu_assert_streq(string.c_str(), "hello, ");
    mu_assert_equal(string.length(), 7);
    string.append("world!");
    mu_assert_streq(string.c_str(), "hello, world!");
    mu_assert_equal(string.length(), 13);
    return 0;
}


mu_test fss_append_overflow() {
    smm::FixedSizeString<8> string("hello, ");
    string.append("world!");
    mu_assert_streq(string.c_str(), "hello, w");
    mu_assert_equal(string.length(), 8);
    return 0;
}


mu_test fss_append_char() {
    smm::FixedSizeString<32> string("hell");
    string.append('o');
    mu_assert_streq(string.c_str(), "hello");
    mu_assert_equal(string.length(), 5);
    return 0;
}


mu_test fss_append_char_multi() {
    smm::FixedSizeString<32> string;
    mu_assert_equal(string.length(), 0);
    string.append('h');
    mu_assert_streq(string.c_str(), "h");
    mu_assert_equal(string.length(), 1);
    string.append('e');
    mu_assert_streq(string.c_str(), "he");
    mu_assert_equal(string.length(), 2);
    string.append('l');
    mu_assert_streq(string.c_str(), "hel");
    mu_assert_equal(string.length(), 3);
    string.append('l');
    mu_assert_streq(string.c_str(), "hell");
    mu_assert_equal(string.length(), 4);
    string.append('o');
    mu_assert_streq(string.c_str(), "hello");
    mu_assert_equal(string.length(), 5);
    return 0;
}


mu_test fss_append_char_overflow() {
    smm::FixedSizeString<4> string("hell");
    string.append('o');
    mu_assert_streq(string.c_str(), "hell");
    mu_assert_equal(string.length(), 4);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * equality operator tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test fss_equality() {
    smm::FixedSizeString<32> string1("hello");
    smm::FixedSizeString<32> string2("hello");
    mu_assert_equal(string1, string2);
    return 0;
}


mu_test fss_cstring_equality() {
    smm::FixedSizeString<32> string("hi!");
    mu_assert_equal(string, "hi!");
    mu_assert_equal("hi!", string);
    return 0;
}


mu_test fss_inequality() {
    smm::FixedSizeString<32> string1("hello");
    smm::FixedSizeString<32> string2("world");
    mu_assert_unequal(string1, string2);
    return 0;
}


mu_test fss_cstring_inequality() {
    smm::FixedSizeString<32> string("hi!");
    mu_assert_unequal(string, "bye!");
    mu_assert_unequal("bye!", string);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * conversion tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test fss_to_int() {
    smm::FixedSizeString<32> string("32");
    mu_assert_equal(string.toInt(), 32);
    return 0;
}


mu_test fss_to_int_negative() {
    smm::FixedSizeString<32> string("-17");
    mu_assert_equal(string.toInt(), -17);
    return 0;
}


mu_test fss_to_int_float() {
    smm::FixedSizeString<32> string("1.618");
    mu_assert_equal(string.toInt(), 1);
    return 0;
}


mu_test fss_to_int_nan() {
    smm::FixedSizeString<32> string("oh brother where art thou");
    mu_assert_equal(string.toInt(), 0);
    return 0;
}


mu_test fss_to_float() {
    smm::FixedSizeString<32> string("1.618");
    mu_assert_equal(string.toFloat(), 1.618f);
    return 0;
}


mu_test fss_to_float_negative() {
    smm::FixedSizeString<32> string("-0.618");
    mu_assert_equal(string.toFloat(), -0.618f);
    return 0;
}


mu_test fss_to_float_int() {
    smm::FixedSizeString<32> string("44");
    mu_assert_equal(string.toFloat(), 44.0f);
    return 0;
}


mu_test fss_to_float_nan() {
    smm::FixedSizeString<32> string("hi there");
    mu_assert_equal(string.toFloat(), 0);
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FixedSizeStringTests() {
    printf("running tests for FixedSizeString\n");
    mu_run_test("create and allocate space for string", fss_create);
    mu_run_test("initialize string value", fss_initialize);
    mu_run_test("string initialization overflow", fss_initialize_overflow);
    
    mu_run_test("append to string", fss_append);
    mu_run_test("append multiple times to string", fss_append_multi);
    mu_run_test("overflow on append to string", fss_append_overflow);
    mu_run_test("append character to string", fss_append_char);
    mu_run_test("append many characters to string", fss_append_char_multi);
    mu_run_test("overflow on append character to string", fss_append_char_overflow);

    mu_run_test("equality operator", fss_equality);
    mu_run_test("equality operator with C strings", fss_cstring_equality);
    mu_run_test("inequality operator", fss_inequality);
    mu_run_test("inequality operator with C strings", fss_cstring_inequality);

    mu_run_test("convert to integer", fss_to_int);
    mu_run_test("convert to negative integer", fss_to_int_negative);
    mu_run_test("convert float string to integer", fss_to_int_float);
    mu_run_test("fail to convert arbitrary string to integer", fss_to_int_nan);
    
    mu_run_test("convert to float", fss_to_float);
    mu_run_test("convert to negative float", fss_to_float_negative);
    mu_run_test("convert to float from integer string", fss_to_float_int);
    mu_run_test("fail to convert arbitrary string to float", fss_to_float_nan);
}
