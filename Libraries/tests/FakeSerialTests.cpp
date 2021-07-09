#include "tests.h"
#include "FakeSerial.h"

// this doesn't really do anything, but it won't compile if the Serial
// object doesn't exist.
mu_test fs_begin() {
    Serial.begin(9600);
    return 0;
}


mu_test fs_receive() {
    // new serial object to ensure a clean slate
    _Serial serial;
    // no need to call begin, it's a dummy function
    serial.send("hello");
    mu_assert_equal(serial.available(), 5);
    mu_assert_equal(serial.read(), 'h');
    mu_assert_equal(serial.read(), 'e');
    mu_assert_equal(serial.read(), 'l');
    mu_assert_equal(serial.read(), 'l');
    mu_assert_equal(serial.read(), 'o');
    mu_assert_equal(serial.available(), 0);
    return 0;
}


mu_test fs_transmit() {
    _Serial serial;
    serial.println("hello, world!");
    mu_assert_equal(serial.outbuffer, "hello, world!\n");
    return 0;
}


mu_test fs_transmit_multi() {
    _Serial serial;
    serial.println("hello, world!");
    mu_assert_equal(serial.outbuffer, "hello, world!\n");
    serial.println("goodnight!");
    mu_assert_equal(serial.outbuffer, "hello, world!\ngoodnight!\n");
    return 0;
}


mu_test fs_receive_real() {
    Serial.send("hello");
    mu_assert_equal(Serial.available(), 5);
    mu_assert_equal(Serial.read(), 'h');
    mu_assert_equal(Serial.read(), 'e');
    mu_assert_equal(Serial.read(), 'l');
    mu_assert_equal(Serial.read(), 'l');
    mu_assert_equal(Serial.read(), 'o');
    mu_assert_equal(Serial.available(), 0);
    return 0;    
}


mu_test fs_transmit_multi_real() {
    Serial.println("hello, world!");
    mu_assert_equal(Serial.outbuffer, "hello, world!\n");
    Serial.println("goodnight!");
    mu_assert_equal(Serial.outbuffer, "hello, world!\ngoodnight!\n");
    return 0;
}


mu_test fs_reset() {
    Serial.reset();
    mu_assert_equal(Serial.available(), 0);
    mu_assert_equal(Serial.outbuffer, "");
    Serial.println("hello, world!");
    Serial.send("goodbye!");
    mu_assert_equal(Serial.outbuffer, "hello, world!\n");
    mu_assert_equal(Serial.available(), 8);
    Serial.reset();
    mu_assert_equal(Serial.available(), 0);
    mu_assert_equal(Serial.outbuffer, "");
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FakeSerialTests() {
    int tests_run_old = tests_run;
    
    printf("running tests for fake Serial\n");
    mu_run_test("receive data", fs_receive);
    mu_run_test("transmit data", fs_transmit);
    mu_run_test("transmit multiple lines of data", fs_transmit_multi);

    mu_run_test("receive data on Serial object", fs_receive_real);
    mu_run_test("transmit multiple lines of data on Serial object", fs_transmit_multi_real);
    mu_run_test("reset Serial object", fs_reset);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
