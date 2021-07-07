#pragma once

#include <string.h>
#include <stdlib.h>

namespace smm {
    template<size_t MAX_LENGTH>
    class FixedSizeString {
    public:
	FixedSizeString() { clear(); }
	FixedSizeString(const char *str) {
	    clear();
	    strncpy(m_str, str, MAX_LENGTH);
	      m_length = strlen(m_str);
	}

	void clear() {
	    memset(m_str, 0, MAX_LENGTH+1);
	    m_length = 0;
	}

	void append(const char *string) {
	    strncat(m_str, string, MAX_LENGTH - m_length);
	      m_length = strlen(m_str);
	}

	void append(char c) {
	    if (m_length < (MAX_LENGTH-1)) {
		m_str[m_length] = c;
		m_str[m_length+1] = 0;
		m_length++;
	    }
	}

	int toInt() {
	    return atoi(m_str);
	}

	float toFloat() {
	    return atof(m_str);
	}

	const char * c_str() { return m_str; }

	inline friend bool operator==(FixedSizeString& lhs, const char *rhs) {
	    return strcmp(lhs.c_str(), rhs) == 0;
	}
	inline friend bool operator!=(FixedSizeString& lhs, const char *rhs) {
	    return !(lhs == rhs);
	}
	inline friend bool operator==(const char *lhs, FixedSizeString& rhs) {
	    return rhs == lhs;
	}
	inline friend bool operator!=(const char *lhs, FixedSizeString& rhs) {
	    return rhs != lhs;
	}
	inline friend bool operator==(FixedSizeString& lhs, FixedSizeString& rhs) {
	    return lhs == rhs.c_str();
	}
	inline friend bool operator!=(FixedSizeString& lhs, FixedSizeString& rhs) {
	    return !(lhs == rhs);
	}

	unsigned int length() { return m_length; }

	size_t maxLength() { return MAX_LENGTH; }
	
    private:
	unsigned int m_length;
	char m_str[MAX_LENGTH+1];
    };
}
