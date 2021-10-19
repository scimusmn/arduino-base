#include "tests.h"
#include "Rfid/Controller.h"
#include "Rfid/ID12LA.h"
#include "Rfid/Reader/RfidReader.hpp"

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


mu_test elt_save_load_rfid() {
    EEPROM.clear();

    smm::EEPROMLookupTable<smm::RfidTag, char, 16> tbl1;
    mu_assert_equal(tbl1.size(), 0);
    mu_assert_equal(tbl1.maxSize(), 16);
    smm::RfidTag tag1(0x01, 0x01, 0x01, 0x01, 0x01);
    smm::RfidTag tag1_2(0x01, 0x01, 0x01, 0x01, 0x01);
    smm::RfidTag tag2(0x02, 0x02, 0x02, 0x02, 0x02);
    smm::RfidTag tag2_2(0x02, 0x02, 0x02, 0x02, 0x02);
    smm::RfidTag tag3(0x03, 0x03, 0x03, 0x03, 0x03);
    smm::RfidTag tag3_2(0x03, 0x03, 0x03, 0x03, 0x03);
    tbl1.add(tag1, '1');
    tbl1.add(tag2, '2');
    tbl1.add(tag3, '3');
    mu_assert_equal(tbl1.size(), 3);
    mu_assert_equal(*(tbl1[tag1_2]), '1');
    mu_assert_equal(*(tbl1[tag2_2]), '2');
    mu_assert_equal(*(tbl1[tag3_2]), '3');
    tbl1.save();

    smm::EEPROMLookupTable<smm::RfidTag, char, 16> tbl2;
    tbl2.load();
    mu_assert_equal(tbl2.size(), 3);
    mu_assert_equal(*(tbl2[tag1_2]), '1');
    mu_assert_equal(*(tbl2[tag2_2]), '2');
    mu_assert_equal(*(tbl2[tag3_2]), '3');

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


mu_test rftag_checksum() {
    smm::RfidTag tag( 0x0c, 0x00, 0x06, 0x21, 0xa5 );
    mu_assert_equal(tag.checksum(), 0x8e);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ID12LA tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

smm::RfidTag testTag;
void onReadTag(smm::RfidTag& tag) {
    testTag = tag;
}

#define STX "\x02"
#define ETX "\r\n\x03"

mu_test id12la_rx_normal() {
    smm::ID12LA reader;
    reader.setup(2, onReadTag);

    SoftwareSerial *serial = reader.getSerial();
    serial->send(STX "0C000621A58E" ETX);
    reader.update();

    smm::RfidTag expected( 0x0c, 0x00, 0x06, 0x21, 0xa5 );
    mu_assert_equal(testTag, expected);
    
    return 0;
}


mu_test id12la_rx_multi() {
    smm::ID12LA reader;
    reader.setup(2, onReadTag);

    SoftwareSerial *serial = reader.getSerial();
    serial->send(STX "0C000621A58E" ETX);
    reader.update();

    smm::RfidTag expected( 0x0c, 0x00, 0x06, 0x21, 0xa5 );
    mu_assert_equal(testTag, expected);

    serial->send(STX "05AAE062B499" ETX);
    reader.update();

    expected = smm::RfidTag(0x05, 0xaa, 0xe0, 0x62, 0xb4);
    mu_assert_equal(testTag, expected);
    
    return 0;
}


mu_test id12la_rx_corrupted() {
    smm::ID12LA reader;
    reader.setup(2, onReadTag);

    SoftwareSerial *serial = reader.getSerial();
    serial->send(STX "0C000621A58E" ETX);
    reader.update();

    smm::RfidTag expected( 0x0c, 0x00, 0x06, 0x21, 0xa5 );
    mu_assert_equal(testTag, expected);

    // bad checksum
    serial->send(STX "05AAE062B444" ETX);
    reader.update();
    smm::RfidTag nonexpected(0x05, 0xaa, 0xe0, 0x62, 0xb4);
    mu_assert_equal(testTag, expected);
    mu_assert_unequal(testTag, nonexpected);

    // corrupted bit
    serial->send(STX "05AAE072B499" ETX);
    reader.update();
    mu_assert_equal(testTag, expected);
    mu_assert_unequal(testTag, nonexpected);

    // too short
    serial->send(STX "05AAE062B44" ETX);
    reader.update();
    mu_assert_equal(testTag, expected);
    mu_assert_unequal(testTag, nonexpected);

    // correct
    serial->send(STX "05AAE062B499" ETX);
    reader.update();
    smm::RfidTag tmp = expected;
    expected = nonexpected;
    nonexpected = tmp;
    mu_assert_equal(testTag, expected);
    mu_assert_unequal(testTag, nonexpected);
    
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * RfidReader.h tests
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct RfidReader rfid_reader;

mu_test rfidreader_setup_and_read() {
   setupReader();
   SoftwareSerial *serial = rfid_reader.id12la.getSerial();
   mu_assert_equal(digitalRead(LED_PIN), 0);

   _Wire testWire;
   testWire.begin();

   testWire.requestFrom(ADDRESS, 5);
   mu_assert_equal(testWire.available(), 5);
   byte b = testWire.read(); printf("%02x\n", b);
   mu_assert_equal(b, 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.available(), 0);

   serial->send(STX "0C000621A58E" ETX);
   updateReader();
   smm::RfidTag expected( 0x0c, 0x00, 0x06, 0x21, 0xa5 );
   mu_assert_equal(rfid_reader.tag, expected);
   
   testWire.requestFrom(ADDRESS, 5);
   mu_assert_equal(testWire.available(), 5);
   mu_assert_equal(testWire.read(), 0x0c);
   mu_assert_equal(testWire.read(), 0x00);
   mu_assert_equal(testWire.read(), 0x06);
   mu_assert_equal(testWire.read(), 0x21);
   mu_assert_equal(testWire.read(), 0xa5);
   mu_assert_equal(testWire.available(), 0);

   testWire.beginTransmission(ADDRESS);
   testWire.write(CMD_CLEAR_TAG);
   testWire.endTransmission();

   testWire.requestFrom(ADDRESS, 5);
   mu_assert_equal(testWire.available(), 5);
   b = testWire.read(); printf("%02x\n", b);
   mu_assert_equal(b, 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.read(), 0xff);
   mu_assert_equal(testWire.available(), 0);

   return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void RfidTests() {
    int tests_run_old = tests_run;
    printf("running tests for EEPROMLookupTable\n");

    mu_run_test("create EEPROM lookup table", elt_create);
    mu_run_test("save and load lookup table from EEPROM", elt_save_load);
    mu_run_test("save and load lookup table from EEPROM with overflow", elt_save_load_overflow);
    mu_run_test("save and load lookup table with RFID tag keys", elt_save_load_rfid);

    printf("  ran %d tests\n", tests_run - tests_run_old);
    tests_run_old = tests_run;
    printf("running tests for RfidTag\n");

    mu_run_test("create RFID tag", rftag_create);
    mu_run_test("create RFID tag with array constructor", rftag_array_constructor);
    mu_run_test("use index operator for access", rftag_index_operator_access);
    mu_run_test("use index operator for mutation", rftag_index_operator_modify);
    mu_run_test("check equality operator", rftag_equality_operator);
    mu_run_test("convert to string", rftag_tostring);
    mu_run_test("compute checksum", rftag_checksum);
	       
    printf("  ran %d tests\n", tests_run - tests_run_old);
    tests_run_old = tests_run;
    printf("running tests for ID12LA\n");

    mu_run_test("read sample tag", id12la_rx_normal);
    mu_run_test("read multiple tags", id12la_rx_multi);
    mu_run_test("handle corrupted input data", id12la_rx_corrupted);

    mu_run_test("setup and read from RfidReader", rfidreader_setup_and_read);

    printf("  ran %d tests\n", tests_run - tests_run_old);
}
