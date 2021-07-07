#pragma once

#include <stdlib.h>
#include "FixedSizeString.h"

namespace smm {
    template<unsigned int MAX_ENTRIES, typename T, size_t MAX_STR_LEN = 32>
    class LookupTable {
    public:
	LookupTable() {}

	T* lookup(FixedSizeString& string);

    private:
	smm::FixedSizeString<MAX_STR_LEN> strings[MAX_ENTRIES];
	T entries[MAX_ENTRIES];
    };
}
