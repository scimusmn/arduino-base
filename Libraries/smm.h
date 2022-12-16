#pragma once


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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

	virtual const char * what() const { 
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
				// do NOT use this function if the container is full!!
				// any keys not already in the map will overwrite whatever is in
				// position 0
				return m_value[0];
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

	bool full() {
		return m_size == sz;
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
		strncpy(m_str, s, sz-1);
	}

	const char *c_str() {
		return reinterpret_cast<const char*>(m_str);
	}

	void push_back(char ch) {
		if (size() + 1 > sz) {
			// not enough space to store an additional character
			return;
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


#ifndef SMM_NO_SERIAL_CONTROLLER
class SerialController {
	public:
	typedef string<SMM_SERIAL_KEY_LEN> key_string;
	typedef string<SMM_SERIAL_VAL_LEN> val_string;

	protected:
	static int s_numCallbacks;
	static const char * s_key[SMM_SERIAL_MAX_CALLBACKS];
	static SerialCallback * s_cb[SMM_SERIAL_MAX_CALLBACKS];

	static int FindCallback(const char *k) {
		for (int i=0; i<s_numCallbacks; i++) {
			if (strcmp(k, s_key[i]) == 0) {
				return i;
			}
		}

		return -1;
	}

	key_string m_key;
	unsigned int m_keyOverflow;
	val_string m_val;
	unsigned int m_valOverflow;

	enum {
		WAIT_FOR_PACKET,
		READ_KEY,
		READ_VAL,
	} state;

	void eatPacketChar(char c) {
		switch (c) {
		case '{':
			state = READ_KEY;
			break;
		default:
			// ignore
			break;
		}
	}

	void eatKeyChar(char c) {
		switch(c) {
		case '{':
			// start new packet
			reset();
			state = READ_KEY;
			break;
		case '}':
			// done too early, wait for new packet
			reset();
			break;
		case ':':
			// key finished, read value
			state = READ_VAL;
			break;
		default:
			if (m_key.size() < m_key.max_size()) {
				m_key.push_back(c);
			}
			else {
				m_keyOverflow += 1;
			}
			break;
		}
	}

	void eatValChar(char c) {
		switch(c) {
		case '{':
			// start new packet
			reset();
			state = READ_KEY;
			break;
		case '}':
			// done! 
			ExecuteCallback(m_key.c_str(), m_val.c_str());
			reset();
			break;
		case ':':
			// ??? this should not be here, reset
			reset();
			break;
		default:
			if (m_val.size() < m_val.max_size()) {
				m_val.push_back(c);
			}
			else {
				m_valOverflow += 1;
			}
			break;
		}
	}


	void reset() {
		state = WAIT_FOR_PACKET;
		m_key = "";
		m_val = "";
		m_keyOverflow = 0;
		m_valOverflow = 0;
	}



	public:
	static bool RegisterCallback(const char *name, SerialCallback *cb) {
		int index = FindCallback(name);
		if (index < 0) {
			index = s_numCallbacks;
			s_numCallbacks += 1;
			s_key[index] = name;
			s_cb[index] = cb;
			return true;
		}
		else {
			// already have a callback with that name, return false
			return false;
		}
	}

	static void ExecuteCallback(const char *key, const char *value) {
		int i = FindCallback(key);
		if (i < 0) {
			// do error things
		}
		else {
			(s_cb[i])->operator()(value);
		}
	}

	SerialController() {
		reset();
	}

	void eatCharacter(char c) {
		switch (state) {
		case WAIT_FOR_PACKET:
			eatPacketChar(c);
			break;
		case READ_KEY:
			eatKeyChar(c);
			break;
		case READ_VAL:
			eatValChar(c);
			break;
		default:
			state = WAIT_FOR_PACKET;
			break;
		}
	}

	static size_t num_callbacks() {
		return s_numCallbacks;
	}
};
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::Button
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef SMM_MAX_BUTTONS
#define SMM_MAX_BUTTONS 16
#endif 

class Button {
	protected:
	static map<int, Button*, SMM_MAX_BUTTONS> s_bindings;
	bool m_pressed;
	unsigned long m_lastTime;
	unsigned long m_debounceTime;

	public:
	template <int pin>
	void setup(unsigned long debounceTime=20) {
		m_debounceTime = debounceTime;
		s_bindings[pin] = this;
		m_pressed = false;
		m_lastTime = 0;
		attachInterrupt(digitalPinToInterrupt(pin), smm::Button::__change<pin>, CHANGE);
	}

	virtual void onPress() {}
	virtual void onRelease() {}

	template <int pin>
	static void __change() {
		if (!s_bindings.contains(pin)) {
			// no button bound
			// not sure how this interrupt was attached but we should ignore
			return;
		}

		Button *b = s_bindings[pin];

		if ((millis() - b->m_lastTime) < b->m_debounceTime) {
			// still debouncing, ignore
			return;
		}

		bool state = !digitalRead(pin);
		if (b->m_pressed == state) {
			// no change in state, ignore
			return;
		}

		b->m_pressed = state;
		b->m_lastTime = millis();
		if (b->m_pressed)
			b->onPress();
		else
			b->onRelease();
	}
};


}


extern smm::SerialController SmmSerial;


#define SERIAL_CAT_(x, y) x##y
#define SERIAL_CAT(x, y) SERIAL_CAT_(x, y)
#define SERIAL_ANONYMOUS(prefix) SERIAL_CAT(prefix, __LINE__)
#define SERIAL_CALLBACK_REGISTER(cb, var, f, name) \
static smm::SerialCallback cb(f); \
static const bool var = SmmSerial.RegisterCallback(name, &cb);

#define SERIAL_CALLBACK_(f, name, arg) \
void f(arg); \
SERIAL_CALLBACK_REGISTER(SERIAL_ANONYMOUS(SERIAL_CALLBACK_CB_), SERIAL_ANONYMOUS(SERIAL_CALLBACK_VAR_), f, name); \
void f(arg)

#define SERIAL_CALLBACK(name, arg) SERIAL_CALLBACK_(SERIAL_ANONYMOUS(SERIAL_CALLBACK_), name, arg)


#ifdef SMM_IMPLEMENTATION
#ifndef SMM_NO_SERIAL_CONTROLLER
/* static variable declarations */
int smm::SerialController::s_numCallbacks = 0;
const char * smm::SerialController::s_key[SMM_SERIAL_MAX_CALLBACKS];
smm::SerialCallback * smm::SerialController::s_cb[SMM_SERIAL_MAX_CALLBACKS];

/* extern globals */
smm::SerialController SmmSerial;
#endif

smm::map<int, smm::Button*, SMM_MAX_BUTTONS> smm::Button::s_bindings;
#endif
