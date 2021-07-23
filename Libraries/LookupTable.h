#pragma once

#include <stdlib.h>
#include "FixedSizeString.h"

namespace smm {
    template<typename keyT, typename valT, unsigned int MAX_ENTRIES=16>
    class LookupTable {
    public:
	/** (constructor) */
	LookupTable() : m_numEntries(0) {}
	
	
	/** look up a particular entry in the table.
	 *
	 * @param[in] key The key for the entry to look up.
	 *
	 * @returns A pointer to the requested entry if it exists, and `nullptr` otherwise.
	 */
	valT* operator[](keyT key) {
	    // this table is intended for small (<32) sizes, so a simple for loop
	    // should be fine, no need for anything fancy
	    for (int i=0; i<m_numEntries; i++) {
		if (m_keys[i] == key) {
		    return m_values + i;
		}
	    }
	    return nullptr;
	}

	
	/** add an entry to the table
	 *
	 * @param[in] key The key for the new entry
	 * @param[in] value The value for the new entry
	 *
	 * @returns True if the entry was added successfully and false otherwise. */
	bool add(keyT key, valT value) {
	    if (m_numEntries < MAX_ENTRIES) {
		int i = m_numEntries;
		m_keys[i] = key;
		m_values[i] = value;
		m_numEntries++;
		return true;
	    }
	    else { return false; }
	}

	
	/** get the current number of table entries
	 *
	 * @returns The number of entries currently in the table. */
	unsigned int size() { return m_numEntries; }
	

	/** get the maximum possible number of table entries
	 *
	 * @returns The maximum possible number of entries the table can hold. */
	unsigned int maxSize() { return MAX_ENTRIES; }

	keyT key(int index) { return m_keys[index]; }
	valT value(int index) { return m_values[index]; }

    private:
	unsigned int m_numEntries;
	keyT m_keys[MAX_ENTRIES];
	valT m_values[MAX_ENTRIES];
    };
}
