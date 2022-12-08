#pragma once

#include <stdlib.h>
#include <string.h>
#include "exception.hpp"

namespace smm
{

template <size_t sz>
class string {
	protected:
	char m_str[sz];

	public:
	string() {}

	string(const char *s) {
		operator=(s);
	}

	void operator=(const char* s) {
		size_t len = strlen(s) + 1;
		if (len > sz) {
			// not enough space to store this string
			throw smm::out_of_memory("not enough space to copy string");
		}
		strncpy(m_str, s, sz);
	}

	const char *c_str() {
		return reinterpret_cast<const char*>(m_str);
	}
};

}
