#include "tests.h"
#include "../Rfid.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * EEPROMLookupTable tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test elt_create() {
    smm::EEPROMLookupTable<smm::String32, char, 16> tbl;
    mu_assert_equal(tbl.size(), 0);
    mu_assert_equal(tbl.maxSize(), 16);
    return 0;
}


mu_test elt_save_load() {
    EEPROM.clear();
    smm::EEPROMLookupTable<smm::String32, char, 16> tbl;
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

    smm::EEPROMLookupTable<smm::String32, char, 16> tbl2;
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


mu_test elt_save_load_overflow() {
    EEPROM.clear();
    smm::EEPROMLookupTable<smm::String32, char, 16> tbl;
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

    smm::EEPROMLookupTable<smm::String32, char, 2> tbl2;
    mu_assert_equal(tbl2.size(), 0);
    mu_assert_equal(tbl2.maxSize(), 2);
    tbl2.load();
    mu_assert_equal(tbl2.size(), 2);
    mu_assert_equal(tbl2.maxSize(), 2);
    mu_assert_equal(*(tbl2["aaaaa"]), 'a');
    mu_assert_equal(*(tbl2["bbbbb"]), 'b');
    mu_assert_equal(tbl2["ccccc"], nullptr);

    smm::EEPROMLookupTable<smm::String32, char, 16> tbl3;
    mu_assert_equal(tbl3.size(), 0);
    mu_assert_equal(tbl3.maxSize(), 16);
    tbl3.load();
    mu_assert_equal(tbl3.size(), 3);
    mu_assert_equal(tbl3.maxSize(), 16);
    mu_assert_equal(*(tbl3["aaaaa"]), 'a');
    mu_assert_equal(*(tbl3["bbbbb"]), 'b');
    mu_assert_equal(*(tbl3["ccccc"]), 'c');

    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * RfidTag tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

mu_test rftag_create() {
    smm::RfidTag tag;
    tag.tagData[0] = 'a';
    tag.tagData[1] = 'b';
    tag.tagData[2] = 'c';
    tag.tagData[3] = 'd';
    tag.tagData[4] = 'e';

    mu_assert_equal(tag.tagData[0], 'a');
    mu_assert_equal(tag.tagData[1], 'b');
    mu_assert_equal(tag.tagData[2], 'c');
    mu_assert_equal(tag.tagData[3], 'd');
    mu_assert_equal(tag.tagData[4], 'e');

    return 0;
}


mu_test rftag_array_constructor() {
    smm::RfidTag tag( 0x00, 0x01, 0x02, 0x03, 0x04 );

    mu_assert_equal(tag.tagData[0], 0x00);
    mu_assert_equal(tag.tagData[1], 0x01);
    mu_assert_equal(tag.tagData[2], 0x02);
    mu_assert_equal(tag.tagData[3], 0x03);
    mu_assert_equal(tag.tagData[4], 0x04);

    return 0;
}


mu_test rftag_index_operator_access() {
    smm::RfidTag tag( 0x00, 0x01, 0x02, 0x03, 0x04 );

    mu_assert_equal(tag[0], 0x00);
    mu_assert_equal(tag[1], 0x01);
    mu_assert_equal(tag[2], 0x02);
    mu_assert_equal(tag[3], 0x03);
    mu_assert_equal(tag[4], 0x04);

    return 0;    
}


mu_test rftag_index_operator_modify() {
    smm::RfidTag tag( 0x00, 0x01, 0x02, 0x03, 0x04 );

    mu_assert_equal(tag[3], 0x03);
    tag[3] = 0xff;
    mu_assert_equal(tag[3], 0xff);

    return 0;
}


mu_test rftag_equality_operator() {
    smm::RfidTag tag1( 0x00, 0x01, 0x02, 0x03, 0x04 );
    smm::RfidTag tag2( 0xff, 0x01, 0x02, 0x03, 0x04 );
    smm::RfidTag tag3( 0x00, 0x01, 0x02, 0x03, 0x04 );

    mu_assert_equal(tag1, tag3);
    mu_assert_equal(tag3, tag1);
    mu_assert_unequal(tag1, tag2);
    mu_assert_unequal(tag3, tag2);
    mu_assert_unequal(tag2, tag1);
    mu_assert_unequal(tag2, tag3);

    return 0;
}


mu_test rftag_tostring() {
    smm::RfidTag tag( 0x00, 0x01, 0x02, 0x03, 0x04 );

    smm::String16 str = tag.toString();
    mu_assert_equal(str, "00 01 02 03 04");
    return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void RfidTests() {
    int tests_run_old = tests_run;
    printf("running tests for EEPROMLookupTable\n");

    mu_run_test("create EEPROM lookup table", elt_create);
    mu_run_test("save and load lookup table from EEPROM", elt_save_load);
    mu_run_test("save and load lookup table from EEPROM with overflow", elt_save_load_overflow);

    printf("  ran %d tests\n", tests_run - tests_run_old);

    tests_run_old = tests_run;
    printf("running tests for RfidTag\n");

    mu_run_test("create RFID tag", rftag_create);
    mu_run_test("create RFID tag with array constructor", rftag_array_constructor);
    mu_run_test("use index operator for access", rftag_index_operator_access);
    mu_run_test("use index operator for mutation", rftag_index_operator_modify);
    mu_run_test("check equality operator", rftag_equality_operator);
    mu_run_test("convert to string", rftag_tostring);
	       
    printf("  ran %d tests\n", tests_run - tests_run_old);
}
