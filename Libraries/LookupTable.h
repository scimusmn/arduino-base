#pragma once

#include <stdlib.h>
#include "FixedSizeString.h"

namespace smm {
    template<unsigned int MAX_ENTRIES, typename T, size_t MAX_STR_LEN = 32>
    class LookupTable {
    public:
	/** look up a particular entry in the table.
	 *
	 * @param[in] key The key for the entry to look up.
	 *
	 * @returns A pointer to the requested entry if it exists, and `nullptr` otherwise.
	 */
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
	 */
	T* operator[](smm::FixedSizeString<MAX_STR_LEN>& key) { return nullptr; }

	
	/** add an entry to the table
	 *
	 * @param[in] key The key for the new entry
	 * @param[in] value The value for the new entry
	 *
	 * @returns True if the entry was added successfully and false otherwise. */
	bool add(const char *key, T value) { return false; }

	
	/** add an entry to the table
	 * This is an overload of the above function and differs only in the arguments it accepts.
	 * 
	 * @param[in] key The key for the new entry
	 * @param[in] value The value for the new entry
	 *
	 * @returns True if the entry was added successfully and false otherwise. */
	bool add(smm::FixedSizeString<MAX_STR_LEN>& key, T value) { return false; }


	/** get the current number of table entries
	 *
	 * @returns The number of entries currently in the table. */
	unsigned int size() { return m_numEntries; }
	

	/** get the maximum possible number of table entries
	 *
	 * @returns The maximum possible number of entries the table can hold. */
	unsigned int maxSize() { return MAX_ENTRIES; }

    private:
	unsigned int m_numEntries;
	smm::FixedSizeString<MAX_STR_LEN> m_strings[MAX_ENTRIES];
	T m_entries[MAX_ENTRIES];
    };
}
