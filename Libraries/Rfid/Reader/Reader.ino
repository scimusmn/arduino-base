#include "RfidReader.hpp"

using namespace smm;

RfidReader rfid_reader;

void setup() {
   setupReader();
}


void loop() {
   updateReader();
}
