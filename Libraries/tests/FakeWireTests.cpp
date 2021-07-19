#include "tests.h"
#include "FakeWire.h"


class BasicPeripheral : public PeripheralDevice {
    void onReceive(int num_bytes) {}
    void onRequest() {}
};


mu_test fwire_basic_tx() {
    Wire.reset();

    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FakeWireTests() {
    int tests_run_old = tests_run;

    printf("running tests for fake Wire\n");

    mu_run_test("transmit to peripheral", fwire_basic_tx);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
