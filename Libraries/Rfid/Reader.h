#pragma once

#ifdef UNIT_TEST
#include "../tests/FakeWire.h"
#include "../tests/FakeEEPROM.h"
_Wire wire;
#else
#include <Wire.h>
#include <EEPROM.h>
#define wire Wire
#endif

#include "ID12LA.h"
#include "Tag.h"

struct {
    extern smm::ID12LA reader;
    extern smm::RfidTag tag;
} rfidData;

#define CMD_SET_ADDRESS 0x44
#define CMD_CLEAR 0x54


namespace smm {
    class RfidReader {
    public:
	
    };
}

void onCommand(int numBytes) {
    if (numBytes < 1)
	return;
    switch(wire.read()) {
    case CMD_SET_ADDRESS:
	if (wire.available())
	    EEPROM.update(0, wire.read());
	break;

    case CMD_CLEAR:
	rfidData.tag.tagData[0] = 0xff;
	rfidData.tag.tagData[1] = 0xff;
	rfidData.tag.tagData[2] = 0xff;
	rfidData.tag.tagData[3] = 0xff;
	rfidData.tag.tagData[4] = 0xff;
	break;

    default:
	// do nothing
	break;
    }
}


void sendTag() {
    wire.write(rfidData.tag.tagData[0]);
    wire.write(rfidData.tag.tagData[1]);
    wire.write(rfidData.tag.tagData[2]);
    wire.write(rfidData.tag.tagData[3]);
    wire.write(rfidData.tag.tagData[4]);
    wire.write(rfidData.tag.checksum());
}


void onTag(smm::RfidTag& t) {
    memcpy(rfidData.tag.tagData, t.tagData, 5 * sizeof(unsigned char));
}


void rfidSetup(int rxPin, unsigned char defaultAddress = 0x70) {
    unsigned char address = EEPROM[0];
    if (address == 0xff)
	address = defaultAddress;
    wire.begin(address);
    wire.onReceive(onCommand);
    wire.onRequest(sendTag);

    rfidReader.reader.setup(rxPin, onTag);

    rfidData.tag.tagData[0] = 0xff;
    rfidData.tag.tagData[1] = 0xff;
    rfidData.tag.tagData[2] = 0xff;
    rfidData.tag.tagData[3] = 0xff;
    rfidData.tag.tagData[4] = 0xff;
}
