#pragma once


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


namespace smm
{


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


	void begin(unsigned long baudrate=115200) {
		Serial.begin(115200);
	}

	void update() {
		while (Serial.available()) {
			eatCharacter(Serial.read());
		}
	}

	void send(const char *key, char *value) {
		Serial.print("{");
		Serial.print(key);
		Serial.print(":");
		Serial.print(value);
		Serial.println("}");
	}

	void send(const char *key, int value) {
		char v[SMM_SERIAL_VAL_LEN];
		snprintf(v, SMM_SERIAL_VAL_LEN, "%d", value);
		send(key, v);
	}

	void send(const char *key, float value) {
		char v[SMM_SERIAL_VAL_LEN];
		snprintf(v, SMM_SERIAL_VAL_LEN, "%f", value);
		send(key, v);
	}
};
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::Button
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef SMM_NO_BUTTON

#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
/* arduino mega */
#define SMM_PCINT_MEGA

#elif defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48PA__) || \
	defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88PA__) || \
	defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || \
	defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
#define SMM_PCINT_328

#else
/* other arduinos */
#define SMM_PCINT_UNKNOWN

#endif

class PcInterruptPort;
class Button;

class PcInterruptManager {
	public:
	#if defined(SMM_PCINT_MEGA)
	#ifndef SMM_PCINT_NO_PORTB
	static PcInterruptPort portB;
	#endif
	#ifndef SMM_PCINT_NO_PORTJ
	static PcInterruptPort portJ;
	#endif
	#ifndef SMM_PCINT_NO_PORTK
	static PcInterruptPort portK;
	#endif
	#elif defined(SMM_PCINT_328)
	#ifndef SMM_PCINT_NO_PORTB
	static PcInterruptPort portB;
	#endif
	#ifndef SMM_PCINT_NO_PORTC
	static PcInterruptPort portC;
	#endif
	#ifndef SMM_PCINT_NO_PORTD
	static PcInterruptPort portD;
	#endif
	#endif

	static void AddButton(int pin, Button *b);
};

class Button {
	protected:
	volatile Button *m_next;
	volatile uint8_t m_mask;
	volatile unsigned long m_debounceTime;
	volatile unsigned long m_lastTime;
	volatile bool m_inverted;
	volatile bool m_pressed;
	friend class PcInterruptPort;

	public:
	Button(int pin, unsigned long debounceTime=5, bool inverted=false) {
		m_next = nullptr;
		m_mask = digitalPinToBitMask(pin);
		m_debounceTime = debounceTime;
		m_lastTime = 0;
		m_inverted = inverted;
		m_pressed = false;
		PcInterruptManager::AddButton(pin, this);
	}

	void addButton(Button *b) {
		if (m_next != nullptr) {
			m_next->addButton(b);
		}
		else {
			m_next = b;
		}
	}

	void onChange(int state) {
		if ((millis() - m_lastTime) < m_debounceTime) {
			// still debouncing, ignore
			return;
		}

		bool push = state == 0;
		if (m_pressed == push) {
			// no change in state, ignore
			return;
		}

		m_pressed = push;
		m_lastTime = millis();
		if (m_pressed)
			onPress();
		else
			onRelease();
	}

	virtual void onPress() = 0;
	virtual void onRelease() = 0;
};

class PcInterruptPort {
	protected:
	volatile Button *m_head;
	volatile uint8_t m_pinMask;
	volatile uint8_t m_previousState;
	volatile uint8_t *m_pcmsk;

	public:
	PcInterruptPort(int index, uint8_t *pcmsk) {
		m_pcmsk = pcmsk;
		cli();
		PCICR |= (1<<index);
		sei();
		m_head = nullptr;
		m_pinMask = 0;
		m_previousState = 0xff;
	}

	void addButton(Button *b) {
		if (m_head == nullptr) {
			m_head = b;
		}
		else {
			m_head->addButton(b);
		}
		cli();
		(*m_pcmsk) |= b->m_mask;
		sei();
		m_pinMask |= b->m_mask;
	}

	void onChange(uint8_t state) {
		uint8_t changed = state ^ m_previousState;
		m_previousState = state;

		Button *b = m_head;
		while (b != nullptr) {
				if (changed & b->m_mask) {
				b->onChange(state & b->m_mask);
			}
			b = b->m_next;
		}
	}
};
/* ifndef SMM_NO_BUTTON */
#endif


}


/* extra macros and extern globals */
#ifndef SMM_NO_SERIAL_CONTROLLER
extern smm::SerialController SmmSerial;
#endif


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


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * implementation details
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifdef SMM_IMPLEMENTATION
#ifndef SMM_NO_SERIAL_CONTROLLER
/* static variable declarations */
int smm::SerialController::s_numCallbacks = 0;
const char * smm::SerialController::s_key[SMM_SERIAL_MAX_CALLBACKS];
smm::SerialCallback * smm::SerialController::s_cb[SMM_SERIAL_MAX_CALLBACKS];

/* extern globals */
smm::SerialController SmmSerial;
#endif


#ifndef SMM_NO_BUTTON
/* smm::Button implementation */
void smm::PcInterruptManager::AddButton(int pin, smm::Button *b) {
	pinMode(pin, INPUT_PULLUP);

	uint8_t port = digitalPinToPort(pin);
	uint16_t oport = portOutputRegister(port);

	#if defined(SMM_PCINT_MEGA)
	if (oport == &PORTB) {
		#ifndef SMM_PCINT_NO_PORTB
		portB.addButton(b);
		#endif
	}
	else if (oport == &PORTJ) {
		#ifndef SMM_PCINT_NO_PORTJ
		portJ.addButton(b);
		#endif
	}
	else if (oport == &PORTK) {
		#ifndef SMM_PCINT_NO_PORTK
		portK.addButton(b);
		#endif
	}
	#elif defined(SMM_PCINT_328)
	if (oport == &PORTB) {
		#ifndef SMM_PCINT_NO_PORTB
		portB.addButton(b);
		#endif
	}
	else if (oport == &PORTC) {
		#ifndef SMM_PCINT_NO_PORTC
		portC.addButton(b);
		#endif
	}
	else if (oport == &PORTD) {
		#ifndef SMM_PCINT_NO_PORTD
		portD.addButton(b);
		#endif
	}
	#endif
	#ifndef SMM_PCINT_UNKNOWN
	else {
		Serial.print("\n\n\n\n\n\n\n\n");
		Serial.println("!!!!!!!! WARNING !!!!!!!!");
		Serial.print("No PCINT port for pin ");
		Serial.print(pin);
		Serial.println("; it WILL NOT work as a switch!");
	}
	#else
	Serial.println("\n\n\n\n\n\n\n\n\n\n\n\nThis chipset is currently not supported by smm::Button.");
	#endif
}

#if defined(SMM_PCINT_MEGA)
#ifndef SMM_PCINT_NO_PORTB
smm::PcInterruptPort smm::PcInterruptManager::portB(PCIE0, &PCMSK0);
ISR(PCINT0_vect) {
	uint8_t pins = PINB;
	smm::PcInterruptManager::portB.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTJ
smm::PcInterruptPort smm::PcInterruptManager::portJ(PCIE1, &PCMSK1);
ISR(PCINT1_vect) {
	uint8_t pins = PINJ;
	smm::PcInterruptManager::portJ.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTK
smm::PcInterruptPort smm::PcInterruptManager::portK(PCIE2, &PCMSK2);
ISR(PCINT2_vect) {
	uint8_t pins = PINK;
	smm::PcInterruptManager::portK.onChange(pins);
}
#endif

#elif defined(SMM_PCINT_328)
#ifndef SMM_PCINT_NO_PORTB
smm::PcInterruptPort smm::PcInterruptManager::portB(PCIE0, &PCMSK0);
ISR(PCINT0_vect) {
	uint8_t pins = PINB;
	smm::PcInterruptManager::portB.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTC
smm::PcInterruptPort smm::PcInterruptManager::portC(PCIE1, &PCMSK1);
ISR(PCINT1_vect) {
	uint8_t pins = PINC;
	smm::PcInterruptManager::portC.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTD
smm::PcInterruptPort smm::PcInterruptManager::portD(PCIE2, &PCMSK2);
ISR(PCINT2_vect) {
	uint8_t pins = PIND;
	smm::PcInterruptManager::portD.onChange(pins);
}
#endif

#endif
/* ifndef SMM_NO_BUTTON */
#endif


/* ifdef SMM_IMPLEMENTATION */
#endif
