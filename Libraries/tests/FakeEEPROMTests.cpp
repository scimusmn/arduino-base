#include "tests.h"
#include "FakeEEPROM.h"

mu_test feeprom_read_write() {
    EEPROM.clear();

    mu_assert_equal(EEPROM.read(0), 255);
    EEPROM.write(0, 42);
    mu_assert_equal(EEPROM.read(0), 42);
    return 0;
}


#define eeprom_readwrite(address, value)	\
    mu_assert_equal(EEPROM.read(address), 255); \
    EEPROM.write(address, value);		\
    mu_assert_equal(EEPROM.read(address), value);

mu_test feeprom_read_write_multi() {
    EEPROM.clear();

    eeprom_readwrite(0, 22);
    eeprom_readwrite(1, 45);
    eeprom_readwrite(3, 56);
    eeprom_readwrite(223, 4);
    return 0;
}


mu_test feeprom_read_update() {
    EEPROM.clear();

    mu_assert_equal(EEPROM.read(0), 255);
    EEPROM.update(0, 42);
    mu_assert_equal(EEPROM.read(0), 42);
    return 0;
}


#define eeprom_readupdate(address, value)	\
    mu_assert_equal(EEPROM.read(address), 255); \
    EEPROM.update(address, value);		\
    mu_assert_equal(EEPROM.read(address), value);

mu_test feeprom_read_update_multi() {
    EEPROM.clear();

    eeprom_readupdate(0, 22);
    eeprom_readupdate(1, 45);
    eeprom_readupdate(3, 56);
    eeprom_readupdate(223, 4);
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FakeEEPROMTests() {
    printf("running tests for fake EEPROM\n");
    int tests_run_old = tests_run;

    mu_run_test("read/write cycle to address 0", feeprom_read_write);
    mu_run_test("read/write cycle to multiple addresses", feeprom_read_write_multi);
    mu_run_test("read/update cycle to address 0", feeprom_read_update);
    mu_run_test("read/update cycle to multiple addresses", feeprom_read_update_multi);
    
    printf("  ran %d tests\n", tests_run - tests_run_old);
}
