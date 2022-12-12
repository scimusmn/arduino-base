#pragma once


#include <stdlib.h>
#include <string.h>


namespace smm
{


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::exception
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

class exception {
	protected:
	const char *m_what;

	public:
	exception() : m_what(nullptr) {}

	exception(const char *str) : m_what(str) {}

	virtual const char * what() { 
		if (m_what != nullptr)
			return m_what;
		return "Unknown exception"; 
	}
};


class logic_error : public exception {
	public:
	logic_error() : exception() {}
	logic_error(const char *str) : exception(str) {}
};

class out_of_range : public exception {
	public:
	out_of_range() : exception() {}
	out_of_range(const char *str) : exception(str) {}
};


class runtime_error : public exception {
	public:
	runtime_error() : exception() {}
	runtime_error(const char *str) : exception(str) {}
};

class out_of_memory : public runtime_error {
	public:
	out_of_memory() : runtime_error() {}
	out_of_memory(const char *str) : runtime_error(str) {}
};


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::map
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

template <typename Key, typename T, size_t sz>
class map {
	protected:
	size_t m_size;
	Key m_key[sz];
	T m_value[sz];

	int get_index(const Key& key) {
		// for small datasets (like the n~10 ones this object is intended to operate on)
		// a linear search should be good enough and probably better than the red-black
		// trees that the libcxx implementation uses
		// sort+binary search might be worth it at some point?
		for (int i=0; i<m_size; i++) {
			if (m_key[i] == key) {
				return i;
			}
		}
		return -1;
	}

	public:
	map() {
		m_size = 0;
	}


	T& operator[](const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key, need to create new
			if (m_size >= sz) {
				// container is full, cannot return new reference
				throw smm::out_of_memory("No space remaining in map");
			}

			i = m_size;
			m_size += 1;

			m_key[i] = key;
			return m_value[i];
		}
		else {
			return m_value[i];
		}
	}


	T& at(const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key found, throw error
			throw smm::out_of_range("No matching key found");
		}
		else {
			return m_value[i];
		}
	}


	size_t erase(const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key found, ignore
			return 0;
		}
		else {
			size_t count = m_size - i - 1;
			memmove(m_key+i, m_key+i+1, sizeof(Key) * count);
			memmove(m_value+i, m_value+i+1, sizeof(T) * count);
			m_size -= 1;
			return 1;
		}
	}


	bool empty() {
		return m_size == 0;
	}

	size_t size() {
		return m_size;
	}

	size_t max_size() {
		return sz;
	}
};


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::string
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

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
