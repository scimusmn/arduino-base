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
	public LookupTable<MAX_ENTRIES, char, MAX_STR_LEN> {
    public:
	void load() {}
	void save() {}
    };
}
