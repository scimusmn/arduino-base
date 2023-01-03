/* @INCLUDE */
#include <stdlib.h>
#include <stdio.h>

/* @HEADER */
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


/** @brief Internal helper structure for storing serial callback functions
 *
 * This structure abstracts the different callbacks so that one interface
 * can call many types of function seamlessly.
 */
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

	/** @brief Call the underlying callback
	 *
	 * The value of `value` will be converted from a string into whatever
	 * type is appropriate for the underlying function pointer.
	 *
	 * @param value  Value to pass to the callback function.
	 */
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
/** @brief human- and machine-readable serial communication protocol
 *
 * A global instance of this class named `SmmSerial` is included in `SMM_IMPLEMENTATION`,
 * so in most circumstances you should not need to create your own.
 *
 * You should create and register callbacks for your SerialController with the @link SERIAL_CALLBACK SERIAL_CALLBACK @endlink macro.
 *
 * If you don't need this class, you can disable it by defining the macro `SMM_NO_SERIAL_CONTROLLER`
 * before including `smm.h`. This may save you memory because it uses static variables.
 */
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

	/** @brief get the number of callbacks currently registered
	 * @returns  The number of callbacks currently registered
	 */
	static size_t num_callbacks() {
		return s_numCallbacks;
	}

	/** @brief initialize serial communication
	 *
	 * @param baudrate  The baudrate to use when communicating.
	 */
	void begin(unsigned long baudrate=115200) {
		Serial.begin(115200);
	}

	/** @brief check for and respond to incoming communication 
	 *
	 * This function should be called fairly frequently.
	 */
	void update() {
		while (Serial.available()) {
			eatCharacter(Serial.read());
		}
	}

	/** @brief send a string-valued pair
	 *
	 * @param key  The string key for the message
	 * @param value  The string value for the message
	 */
	void send(const char *key, char *value) {
		Serial.print("{");
		Serial.print(key);
		Serial.print(":");
		Serial.print(value);
		Serial.println("}");
	}

	/** @brief send an integer-valued pair
	 *
	 * @param key  The string key for the message
	 * @param value  The integer value for the message
	 */
	void send(const char *key, int value) {
		char v[SMM_SERIAL_VAL_LEN];
		snprintf(v, SMM_SERIAL_VAL_LEN, "%d", value);
		send(key, v);
	}

	/** @brief send an float-valued pair
	 *
	 * @param key  The string key for the message
	 * @param value  The float value for the message
	 */
	void send(const char *key, float value) {
		char v[SMM_SERIAL_VAL_LEN];
		snprintf(v, SMM_SERIAL_VAL_LEN, "%f", value);
		send(key, v);
	}
};

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

/** @brief Create a new serial callback
 *
 * The basic usage of this macro is like this:
 * ```
 * SERIAL_CALLBACK("my-callback", const char *str) {
 *     Serial.print("received string '");
 *     Serial.print(str);
 *     Serial.println("'");
 * }
 * ```
 *
 * There are four kinds of callbacks: void, string (shown above), int, and float. To define them,
 * you simply need to provide the appropriate datatype in the `arg` field. Note that for void
 * callbacks, you still need to put a `void` in the arg field, like this:
 *
 * ```
 * SERIAL_CALLBACK("some-void-callback", void) {
 *     // do something
 * }
 * ```
 *
 * This macro is *self-registering*. That is, when you create a serial callback this way, it is now
 * completely set up with `SmmSerial` and will respond to incoming messages on the name you gave; 
 * all you have to do is call `SmmSerial.begin()` in your setup function and `SmmSerial.update()` in 
 * your loop function.
 */
#define SERIAL_CALLBACK(name, arg) SERIAL_CALLBACK_(SERIAL_ANONYMOUS(SERIAL_CALLBACK_), name, arg)

/* end ifndef SMM_NO_SERIAL_CONTROLLER */
#endif

/* @EXTERN */
extern smm::SerialController SmmSerial;

/* @IMPLEMENTATION */
#ifndef SMM_NO_SERIAL_CONTROLLER
/* static variable declarations */
int smm::SerialController::s_numCallbacks = 0;
const char * smm::SerialController::s_key[SMM_SERIAL_MAX_CALLBACKS];
smm::SerialCallback * smm::SerialController::s_cb[SMM_SERIAL_MAX_CALLBACKS];

/* extern global */
smm::SerialController SmmSerial;
#endif
/* @END */
