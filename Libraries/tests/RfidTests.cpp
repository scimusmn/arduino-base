#include "tests.h"
#include "../Rfid.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * EEPROMLookupTable tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test elt_create() {
    smm::EEPROMLookupTable<16> tbl;
    mu_assert_equal(tbl.size(), 0);
    mu_assert_equal(tbl.maxSize(), 16);
    return 0;
}


mu_test elt_save_load() {
    EEPROM.clear();
    smm::EEPROMLookupTable<16> tbl;
    mu_assert_equal(tbl.size(), 0);
    mu_assert_equal(tbl.maxSize(), 16);
    tbl.add("aaaaa", 'a');
    tbl.add("bbbbb", 'b');
    tbl.add("ccccc", 'c');
    mu_assert_equal(tbl.size(), 3);
    mu_assert_equal(tbl.maxSize(), 16);
    mu_assert_equal(*(tbl["aaaaa"]), 'a');
    mu_assert_equal(*(tbl["bbbbb"]), 'b');
    mu_assert_equal(*(tbl["ccccc"]), 'c');

    tbl.save();

    smm::EEPROMLookupTable<16> tbl2;
    mu_assert_equal(tbl2.size(), 0);
    mu_assert_equal(tbl2.maxSize(), 16);
    tbl2.load();
    mu_assert_equal(tbl2.size(), 3);
    mu_assert_equal(tbl2.maxSize(), 16);
    mu_assert_equal(*(tbl2["aaaaa"]), 'a');
    mu_assert_equal(*(tbl2["bbbbb"]), 'b');
    mu_assert_equal(*(tbl2["ccccc"]), 'c');
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void RfidTests() {
    int tests_run_old = tests_run;
    printf("running tests for EEPROMLookupTable\n");

    mu_run_test("create EEPROM lookup table", elt_create);
    mu_run_test("save and load lookup table from EEPROM", elt_save_load);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
