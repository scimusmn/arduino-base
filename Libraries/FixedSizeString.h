/** @file FixedSizeString.h */
#pragma once

#include <string.h>
#include <stdlib.h>

namespace smm {
    /** A convenient string-handling class that will not result in memory fragmentation. */
    template<size_t MAX_LENGTH>
    class FixedSizeString {
    public:

	/** (constructor) */
	FixedSizeString() { clear(); }


	/** (constructor)
	 * In case of overflow, the provided string is truncated in the buffer.
	 *
	 * @param[in] str The string to initialize the value to. */
	FixedSizeString(const char *str) {
	    clear();
	    strncpy(m_str, str, MAX_LENGTH);
	    m_length = strlen(m_str);
	}


	/** empty the string.
	 * Clears out the internal buffer and resets length to zero. */
	void clear() {
	    memset(m_str, 0, MAX_LENGTH+1);
	    m_length = 0;
	}


	/** set the value stored in the character buffer
	 * In case of overflow, the provided string is truncated.
	 *
	 * @param[in] string The string to set the buffer to */
	void operator=(const char *string) {
	    strncpy(m_str, string, MAX_LENGTH);
	    m_length = strlen(m_str);
	}

	
	/** set the value stored in the character buffer
	 * This is an overload of the above function; it is provided for convenience.
	 *
	 * @param[in] string The string to set the buffer to */
	template<size_t size>
	void operator=(FixedSizeString<size>& string) { this->operator=(string.c_str()); }

	
	/** set the value stored in the character buffer
	 * This is an overload of the above function; it is provided for convenience.
	 *
	 * @param[in] number The buffer will be set to the base10 representation of this number */
	void operator=(int number) {
	    clear();
	    snprintf(m_str, MAX_LENGTH+1, "%d", number);
	    m_length = strlen(m_str);
	}


	/** set the value stored in the character buffer
	 * This is an overload of the above function; it is provided for convenience.
	 *
	 * @param[in] number The buffer will be set to the base10 representation of this number */
	void operator=(float number) {
	    clear();
	    snprintf(m_str, MAX_LENGTH+1, "%.3f", number);
	    m_length = strlen(m_str);
	}
	    

	/** append to the string
	 * In case of overflow, the string will be truncated.
	 *
	 * @param[in] string The string to append. */
	void append(const char *string) {
	    strncat(m_str, string, MAX_LENGTH - m_length);
	      m_length = strlen(m_str);
	}


	/** append to the string
	 * This is an overload of the above function; it is provided for convenience.
	 *
	 * @param[in] c The character to append. */
	void append(char c) {
	    if (m_length < (MAX_LENGTH)) {
		m_str[m_length] = c;
		m_str[m_length+1] = 0; // ensure a trailing null character
		m_length++;
	    }
	}


	/** convert the string to an integer
	 * This function uses atoi() and so will return 0 if a non-numeric string is provided,
	 * and will provide only the integral component if a floating-point value is provided. */
	int toInt() { return atoi(m_str); }


	/** convert the string to a floating-point number
	 * This function uses atof() and so will return 0 if a non-numeric string is provided. */
	float toFloat() { return atof(m_str); }


	/** get a C string representation
	 * This function returns a pointer to the internal buffer itself; please be careful with it! */
	const char * c_str() { return m_str; }


	/** test string equality */
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


	/** get the length of the string
	 * Note that if you're using unicode or something this will return the length of the string
	 * in *bytes*, not characters.
	 *
	 * @returns The length of the string in bytes */
	unsigned int length() { return m_length; }


	/** get the maximum length of the string
	 * This is the maximum length of a string (in bytes!) that this object can hold, not including
	 * the trailing null character.
	 *
	 * @returns The maximum string length in bytes */
	size_t maxLength() { return MAX_LENGTH; }
	
    private:
	unsigned int m_length;
	char m_str[MAX_LENGTH+1];
    };

    // convenience typedefs
    typedef FixedSizeString<64> String64;
    typedef FixedSizeString<32> String32;
    typedef FixedSizeString<16> String16;
    typedef FixedSizeString<8> String8;
}
