#pragma once

#ifdef UNIT_TEST
#include "tests/FakeEEPROM.h"
#include "tests/FakeWire.h"
#else
#include <EEPROM.h>
#include <Wire.h>
#endif

#include "FixedSizeString.h"
#include "LookupTable.h"
#include "Rfid/RfidTag.h"

typedef unsigned char byte;

namespace smm {
    template<typename keyT, typename valT, unsigned int MAX_ENTRIES>
    class EEPROMLookupTable :
	public smm::LookupTable<keyT, valT, MAX_ENTRIES> {
    public:
	void load() {
	    int numEntries = EEPROM[0];
	    if (numEntries == 255)
		// assume previously unwritten memory cell
		numEntries = 0;

	    if (numEntries > MAX_ENTRIES)
		// only read up to MAX_ENTRIES entries
		numEntries = MAX_ENTRIES;
	    
	    for (int i=0; i<numEntries; i++)
		readEntry(i);
	    
	}
	
	void save() {
	    EEPROM.update(0, this->size());
	    for (int i=0; i<this->size(); i++)
		saveEntry(i);
	}
    private:
	void readEntry(int index) {
	    const int entrySize = sizeof(keyT) + sizeof(valT);
	    int address = 1 + (index * entrySize);

	    keyT key;
	    unsigned char *b = (unsigned char *) &key;
	    for (int i=0; i<sizeof(keyT); i++) {
		*b = EEPROM[address+i];
		b++;
	    }
	    valT value;
	    b = (unsigned char *) &value;
	    for (int i=0; i<sizeof(valT); i++) {
		*b = EEPROM[address+sizeof(keyT)+i];
		b++;
	    }
	    this->add(key, value);
	}

	void saveEntry(int index) {
	    const int entrySize = sizeof(keyT) + sizeof(valT);
	    int address = 1 + (index * entrySize);

	    keyT key = this->key(index);
	    unsigned char *b = (unsigned char *) &key;
	    for (int i=0; i<sizeof(keyT); i++)
		EEPROM.update(address+i, *(b+i));

	    valT value = this->value(index);
	    b = (unsigned char *) &value;
	    for (int i=0; i<sizeof(valT); i++)
		EEPROM.update(address+sizeof(keyT)+i, *(b+i));
	}
    };

    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    typedef void (*readHandler)(RfidTag&);
    

    template<unsigned int MAX_TAGS>
    class RfidController {
    public:
	void setup() {}
	void update() {}

	void addReader(byte address) {}
	void onRead(byte category, readHandler handler) {}
	void teachTag(RfidTag tag, byte category) {}

	void forgetLastTag() {}
    };
}
