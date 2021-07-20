#include "tests.h"
#include "FakeWire.h"

#include <string>


class BasicPeripheral : public PeripheralDevice {
public:
    void onReceive(int num_bytes) {}
    void onRequest() {}
    std::string str;
};


mu_test fwire_basic_tx() {
    Wire.reset();
    BasicPeripheral peripheral;
    Wire.attach(peripheral, 0x70);

    Wire.beginTransmission(0x70);
    Wire.write('a');
    Wire.write('b');
    Wire.write('c');
    Wire.endTransmission();

    mu_assert_equal(peripheral.str, "abc");
    
    return 0;
}


mu_test fwire_basic_rx() {
    Wire.reset();
    BasicPeripheral peripheral;
    Wire.attach(peripheral, 0x70);

    Wire.requestFrom(0x70, 3);
    mu_assert_equal(Wire.available(), 3);
    mu_assert_equal(Wire.read(), 'a');
    mu_assert_equal(Wire.read(), 'b');
    mu_assert_equal(Wire.read(), 'c');

    mu_assert_equal(Wire.available(), 0);

    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FakeWireTests() {
    int tests_run_old = tests_run;

    printf("running tests for fake Wire\n");

    mu_run_test("transmit to peripheral", fwire_basic_tx);
    mu_run_test("receive from peripheral", fwire_basic_rx);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
