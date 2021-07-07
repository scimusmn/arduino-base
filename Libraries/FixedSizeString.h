#pragma once

#include <string.h>

namespace smm {
    template<size_t MAX_SIZE>
    class FixedSizeString {
    public:
	FixedSizeString() { clear(); }
	FixedSizeString(const char *str) {
	    clear();
	    strncpy(m_str, str, MAX_SIZE);
	    m_size = strlen(m_str);
	}

	void clear() {
	    memset(m_str, 0, MAX_SIZE);
	    m_size = 0;
	}

	void append(const char *string) {
	    strncat(m_str, string, MAX_SIZE - m_size);
	    m_size = strlen(m_str);
	}

	void append(char c) {
	    if (m_size < MAX_SIZE-1) {
		m_str[m_size] = c;
		m_str[m_size+1] = 0;
		m_size++;
	    }
	}

	int toInt() {
	    return atoi(m_str);
	}

	float toFloat() {
	    return atof(m_str);
	}

	const char * c_str() { return m_str; }

	inline friend bool operator==(FixedSizeString& lhs, FixedSizeString& rhs) {
	    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
	}
	inline friend bool operator!=(FixedSizeString& lhs, FixedSizeString& rhs) { return !(lhs == rhs); }

	unsigned int size() { return m_size; }

	size_t maxSize() { return MAX_SIZE; }
	
    private:
	unsigned int m_size;
	char m_str[MAX_SIZE];
    };
}
