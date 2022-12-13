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


	bool contains(const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key found
			return false;
		}
		else {
			return true;
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

	friend bool operator==(const string& lhs, const char *rhs) {
		return strcmp(lhs.m_str, rhs) == 0;
	}
	friend bool operator!=(const string& lhs, const char *rhs) { return !(lhs == rhs); }
	friend bool operator==(const string& lhs, const string& rhs) { return lhs == rhs.m_str; }
	friend bool operator!=(const string& lhs, const string& rhs) { return !(lhs == rhs.m_str); }
};



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::SerialController
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef SMM_SERIAL_KEY_LEN
#define SMM_SERIAL_KEY_LEN 128
#endif

#ifndef SMM_SERIAL_VAL_LEN
#define SMM_SERIAL_VAL_LEN 128
#endif

#ifndef SMM_SERIAL_MAX_CALLBACKS
#define SMM_SERIAL_MAX_CALLBACKS 8
#endif


typedef void (*voidCallback)();
typedef void (*stringCallback)(const char *);
typedef void (*intCallback)(int);
typedef void (*floatCallback)(float);


struct SerialCallback {
	union {
		voidCallback v;
		stringCallback s;
		intCallback i;
		floatCallback f;
	} callback;
	enum {
		NONE, VOID_T, STRING, INT, FLOAT,
	} type;


	SerialCallback()                  { type=NONE;                  }
	SerialCallback(voidCallback cb)   { type=VOID_T; callback.v=cb; }
	SerialCallback(stringCallback cb) { type=STRING; callback.s=cb; }
	SerialCallback(intCallback cb)    { type=INT;    callback.i=cb; }
	SerialCallback(floatCallback cb)  { type=FLOAT;  callback.f=cb; }

	void operator()(const char *value) {
		switch(type) {
		case VOID_T:
			callback.v();
			break;

		case STRING:
			callback.s(value);
			break;

		case INT:
			callback.i(strtol(value, NULL, 0));
			break;

		case FLOAT:
			callback.f(strtod(value, NULL));
			break;

		default:
			break;
		}
	}
};


class SerialController {
	public:
	typedef string<SMM_SERIAL_KEY_LEN> key_string;
	typedef string<SMM_SERIAL_VAL_LEN> val_string;

	protected:
	static map<key_string, SerialCallback, SMM_SERIAL_MAX_CALLBACKS> s_callbacks;

	public:
	static bool RegisterCallback(const char *name, SerialCallback cb) {
		key_string name_str(name);
		if (!s_callbacks.contains(name_str)) {
			s_callbacks[name_str] = cb;
			return true;
		}
		else {
			// already have a callback with that name, return false
			return false;
		}
	}

	void ExecuteCallback(const char *key, const char *value) {
		key_string k(key);
		SerialCallback cb = s_callbacks.at(k);
		cb(value);
	}
};

}


#ifdef SMM_IMPLEMENTATION
smm::map<
	smm::string<SMM_SERIAL_KEY_LEN>,
	smm::SerialCallback,
	SMM_SERIAL_MAX_CALLBACKS
> smm::SerialController::s_callbacks;
#endif
