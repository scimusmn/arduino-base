#include "../ID12LA.h"
#include "../Tag.h"

#include <Wire.h>
#include <EEPROM.h>

using namespace smm;

RfidTag tag;
ID12LA reader;

#define CMD_SET_ADDR 0x44
#define CMD_CLEAR_TAG 0x54

void onReadTag(RfidTag t) { memcpy(tag.tagData, t.tagData, 5*sizeof(byte)); }
void sendTag() { Wire.write(tag.tagData, 5); }
void clearTag() { memset(tag.tagData, 0xff, 5 * sizeof(byte)) }
void processCommand(int n) {
    if (n < 1)
	return;

    byte command = Wire.read();
    if (command == CMD_SET_ADDR && n == 2)
	EEPROM.update(0, Wire.read());
    else if (command == CMD_CLEAR_TAG)
	clearTag();
}


void setup() {
    byte address = EEPROM[0];
    if (address == 0xff)
	// assume unwritten memory cell
	address = 0x70;
    Wire.begin(address);
    Wire.onRequest(sendTag);
    Wire.onReceive(processCommand);
}
