#pragma once

#include <stdlib.h>
#include "FixedSizeString.h"

namespace smm {
    template<unsigned int MAX_ENTRIES, typename T, size_t MAX_STR_LEN = 32>
    class LookupTable {
    public:
	LookupTable() {}

	/** look up a particular entry in the table.
	 *
	 * @param[in] key The key for the entry to look up.
	 *
	 * @returns A pointer to the requested entry if it exists, and `nullptr` otherwise.
	 *
	T* operator[](const char *key) {
	    return nullptr;
	}

	/** look up a particular entry in the table.
	 *
	 * This is an overload of the above function and differs only in the arguments it accepts.
	 *
	 * @param[in] key The key for the entry to look up.
	 *
	 * @returns A pointer to the requested entry if it exists, and `nullptr` otherwise.
	 *
	T* operator[](smm::FixedSizeString& key) { return this->operator[](key.c_str()); }
	*/

	/*bool addEntry(smm::FixedSizeString& key, T value);*/

	
	

    private:
	unsigned int m_numEntries;
	smm::FixedSizeString<MAX_STR_LEN> m_strings[MAX_ENTRIES];
	T m_entries[MAX_ENTRIES];
    };
}
