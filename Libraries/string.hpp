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
	string() {
		memset(m_str, 0, sz);
	}

	string(const char *s) {
		memset(m_str, 0, sz);
		operator=(s);
	}

	void operator=(const char* s) {
		size_t len = strlen(s)+1;
		if (len > sz) {
			// not enough space to store this string
			throw smm::out_of_memory("not enough space to copy string");
		}
		strncpy(m_str, s, sz);
	}

	const char *c_str() {
		return reinterpret_cast<const char*>(m_str);
	}

	void push_back(char ch) {
		if (size() + 1 > sz) {
			// not enough space to store an additional character
			throw smm::out_of_memory("not enough space to push character");
		}
		strncat(m_str, &ch, 1);
	}

	size_t size() {
		return strlen(m_str) + 1;
	}

	size_t max_size() {
		return sz;
	}
};

}
