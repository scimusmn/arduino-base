#include "tests.h"
#include "FakeWire.h"

#include <string>


_Wire *pWire; // peripheral Wire object
std::string pString; // peripheral string

void setPeripheralString(int numBytes) {
    pString = "";
    for (int i=0; i<numBytes; i++)
	pString += (char) pWire->read();
}
void sendABC() {
    pWire->write('a');
    pWire->write('b');
    pWire->write('c');
}


mu_test fwire_add_peripheral() {
    mu_except(WireBus.bus.at(0x70));
    pWire = new _Wire;
    pWire->begin(0x70);
    mu_try(WireBus.bus.at(0x70));
    mu_assert_equal(&(WireBus.bus.at(0x70)), pWire);
    delete pWire;
    mu_except(WireBus.bus.at(0x70));
    return 0;
}


mu_test fwire_basic_tx() {
    return 0;
}


mu_test fwire_basic_rx() {
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FakeWireTests() {
    int tests_run_old = tests_run;

    printf("running tests for fake Wire\n");

    mu_run_test("add peripheral to bus", fwire_add_peripheral);
    mu_run_test("transmit to peripheral", fwire_basic_tx);
    mu_run_test("receive from peripheral", fwire_basic_rx);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
