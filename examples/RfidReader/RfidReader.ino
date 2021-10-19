#include <Wire.h>
#include <SoftwareSerial.h>

#include "Tag.h"

smm::RfidTag testTag(0xde, 0xad, 0xbe, 0xee, 0xef);
SoftwareSerial rfidSerial(7, 6);

#define ADDRESS 0x75

bool check_addr(byte addr) {
   Serial.print("check address 0x");
   Serial.println(addr, HEX);
   
   Wire.requestFrom(ADDRESS, 6);
   Serial.print("requested 6 bytes, received ");
   int n_available = Wire.available();
   Serial.println(n_available);

   if (n_available == 0) {
      return false;
   }

   Serial.println("bytes received:");

   while(Wire.available())
      Serial.println(Wire.read(), HEX);

   return true;
}


void scan() {
   for (int addr=0x10; addr<=0xff; addr++) {
      check_addr(addr);
   }
}


void setup() {
   Serial.begin(115200);
   rfidSerial.begin(9600);
   Wire.begin();

   delay(1000);

   if (!check_addr(ADDRESS)) {
      Serial.println("no response on primary address, scanning...");
      scan();
   }
   else {
      Serial.println("clearing tag...");
      Wire.beginTransmission(ADDRESS);
      Wire.write(0x54);
      Wire.endTransmission(ADDRESS);

      check_addr(ADDRESS);
      
      Serial.println("sending test tag...");      

      rfidSerial.write(0x02); // STX
      rfidSerial.write("DEADBEEEEFCC\r\n");
      rfidSerial.write(0x03); // ETX
      
      delay(100);

      check_addr(ADDRESS);

      Serial.println("clearing tag...");
      Wire.beginTransmission(ADDRESS);
      Wire.write(0x54);
      Wire.endTransmission(ADDRESS);

      check_addr(ADDRESS);
   }
}

void loop() {}
