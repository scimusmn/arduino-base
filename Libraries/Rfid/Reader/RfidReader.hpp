#pragma once

#ifdef UNIT_TEST
#include "tests/FakeArduino.h"
#include "tests/FakeWire.h"
#include <stdio.h>
_Wire Wire;
#else
#include <Arduino.h>
#include <Wire.h>
#endif

#include "Rfid/ID12LA.h"
#include "Rfid/Tag.h"

using namespace smm;

#define ADDRESS 0x75

#define LED_PIN 9

#define CMD_SET_ADDR 0x44
#define CMD_CLEAR_TAG 0x54


struct RfidReader {
   RfidTag tag;
   ID12LA id12la;
};

extern struct RfidReader rfid_reader;


void onTag(RfidTag& t) {
   memcpy(rfid_reader.tag.tagData, t.tagData, 5*sizeof(byte));
   digitalWrite(LED_PIN, HIGH);
   delay(100);
   digitalWrite(LED_PIN, LOW);
}
void sendTag() {
   Wire.write(rfid_reader.tag.tagData[0]);
   Wire.write(rfid_reader.tag.tagData[1]);
   Wire.write(rfid_reader.tag.tagData[2]);
   Wire.write(rfid_reader.tag.tagData[3]);
   Wire.write(rfid_reader.tag.tagData[4]);
}

void clearTag() {
   rfid_reader.tag.tagData[0] = 0xff;
   rfid_reader.tag.tagData[1] = 0xff;
   rfid_reader.tag.tagData[2] = 0xff;
   rfid_reader.tag.tagData[3] = 0xff;
   rfid_reader.tag.tagData[4] = 0xff;
}

void processCommand(int n) {
   printf("processing...\n");
   if (n < 1)
      return;

   byte command = Wire.read();
   printf("command: %02x\n", command);
   if (command == CMD_CLEAR_TAG)
      clearTag();
}


void setupReader() {
   pinMode(LED_PIN, OUTPUT);
   Wire.begin(ADDRESS);
   Wire.onRequest(sendTag);
   Wire.onReceive(processCommand);

   clearTag();

   printf("tag: %s\n", rfid_reader.tag.toString().c_str());

   pinMode(LED_PIN, OUTPUT);

   rfid_reader.id12la.setup(0, onTag);
}
void updateReader() {
   rfid_reader.id12la.update();
}
