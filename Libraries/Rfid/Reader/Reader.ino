#include "ID12LA.h"
#include "Tag.h"

#include <Wire.h>

using namespace smm;

RfidTag tag;
ID12LA reader;

#define ADDRESS 0x75

#define LED_PIN 9

#define CMD_SET_ADDR 0x44
#define CMD_CLEAR_TAG 0x54

void onReadTag(RfidTag t) {
   memcpy(tag.tagData, t.tagData, 5*sizeof(byte));
   digitalWrite(LED_PIN, HIGH);
   delay(100);
   digitalWrite(LED_PIN, LOW);
}
void sendTag() { Wire.write(tag.tagData, 5); }
void clearTag() { memset(tag.tagData, 0xff, 5 * sizeof(byte)); }
void processCommand(int n) {
    if (n < 1)
	return;

    byte command = Wire.read();
    if (command == CMD_CLEAR_TAG)
	clearTag();
}


void setup() {
    Wire.begin(ADDRESS);
    Wire.onRequest(sendTag);
    Wire.onReceive(processCommand);

    pinMode(LED_PIN, OUTPUT);

    reader.setup(0, onReadTag);
}


void loop() {
   reader.update();
}
