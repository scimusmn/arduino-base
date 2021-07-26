#pragma once

#ifdef UNIT_TEST
#include "../tests/FakeEEPROM.h"
#include "../tests/FakeWire.h"
#else
#include <EEPROM.h>
#include <Wire.h>
#endif

#include "../FixedSizeString.h"
#include "../LookupTable.h"
#include "Tag.h"

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
    

    template<unsigned int MAX_READERS,
	     unsigned int MAX_TAGS,
	     unsigned int MAX_CATEGORIES>
    class RfidController {
    public:
	void setup() {
	    tags.load();
	    numReaders = 0;
	}
	void update() {
	    for (int i=0; i<numReaders; i++)
		updateReader(readers[i]);
	}

	void addReader(byte address) {
	    readers[numReaders] = address;
	    numReaders++;
	}
	void onRead(byte category, readHandler handler) {
	    handlers.add(category, handler);
	}
	void teachTag(RfidTag tag, byte category) {
	    tags.add(tag, category);
	    tags.save();
	}

	void forgetLastTag() {
	    byte newSize = tags.size() - 1;
	    if (newSize < 0)
		newSize = 0;
	    EEPROM[0] = newSize;
	    tags.load();
	}
    private:
	EEPROMLookupTable<RfidTag, byte, MAX_TAGS> tags;
	LookupTable<char, readHandler, MAX_CATEGORIES> handlers;
	int numReaders;
	byte readers[MAX_READERS];
    };
}
