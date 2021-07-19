#pragma once

#ifdef UNIT_TEST
#include "tests/FakeEEPROM.h"
#else
#include <EEPROM.h>
#endif

#include "LookupTable.h"

namespace smm {
    template<unsigned int MAX_ENTRIES, size_t MAX_STR_LEN = 5>
    class EEPROMLookupTable :
	public smm::LookupTable<MAX_ENTRIES, char, MAX_STR_LEN> {
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
	    const int entrySize = (MAX_STR_LEN * sizeof(char)) + 1;
	    int address = 1 + (index * entrySize);

	    smm::FixedSizeString<MAX_STR_LEN> str;
	    for (int i=0; i<MAX_STR_LEN; i++)
		str.append(EEPROM[address + i]);
	    this->add(str.c_str(), EEPROM[address+MAX_STR_LEN]);
	}

	void saveEntry(int index) {
	    const int entrySize = (MAX_STR_LEN * sizeof(char)) + 1;
	    int address = 1 + (index * entrySize);

	    const char *str = this->key(index);;
	    for (int i=0; i<MAX_STR_LEN; i++)
		EEPROM.update(address+i, str[i]);
	    EEPROM.update(address+MAX_STR_LEN, this->value(index));
	}
    };
}
