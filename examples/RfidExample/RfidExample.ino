#include "Rfid.h"

struct Rfid *rfid;

enum { TAG_KNOWN,
       N_CATEGORIES
};


void onread_TAG_KNOWN(byte ra, byte id[5], void* d) {
    Serial.print("Read known tag [");
    Serial.print(idToString(id));
    Serial.print("] on reader ");
    Serial.println(ra, HEX);
}


void onread_unknown(byte ra, byte id[5], void* d) {
    Serial.print("Read unknown tag [");
    Serial.print(idToString(id));
    Serial.print("] on reader ");
    Serial.println(ra, HEX);
}

void setup() {
    Serial.begin(9600);

    Serial.print("set up rfid...");
    
    rfid = rfidSetup(N_CATEGORIES, 13, true);
    rfidAddReader(rfid, 0x70);
    rfidAddCategory(rfid, TAG_KNOWN, 2, onread_TAG_KNOWN);

    rfid->onunknown = onread_unknown;

    Serial.println(" done.");

    Serial.print("Known tags: ");
    Serial.println(rfid->numTags);
}

	
void loop() {
    rfidUpdate(rfid);
}
