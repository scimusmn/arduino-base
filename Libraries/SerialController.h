#pragma once

#ifdef UNIT_TEST
#include "tests/FakeSerial.h"
#else
#include <Arduino.h>
#endif

#include "FixedSizeString.h"
#include "LookupTable.h"

namespace smm {

    typedef void (*voidCallback)();
    typedef void (*stringCallback)(const char*);
    typedef void (*intCallback)(int);
    typedef void (*floatCallback)(float);


    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *
     * SerialCallback
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    struct SerialCallback {
	SerialCallback() { valueType = NONE; }
	SerialCallback(voidCallback cb)   { valueType = VOID;   callback.v = cb; }
	SerialCallback(stringCallback cb) { valueType = STRING; callback.s = cb; }
	SerialCallback(intCallback cb)    { valueType = INT;    callback.i = cb; }
	SerialCallback(floatCallback cb)  { valueType = FLOAT;  callback.f = cb; }

	typedef enum {
	    NONE,
	    VOID,
	    STRING,
	    INT,
	    FLOAT
	} ValueType;

	ValueType valueType;

	union {
	    voidCallback v;
	    stringCallback s;
	    intCallback i;
	    floatCallback f;
	} callback;

	void operator()(const char *value) {
	    switch (valueType) {
	    case NONE:
		break;
		
	    case VOID:
		callback.v();
		break;
      
	    case STRING:
		callback.s(value);
		break;

	    case INT:
		callback.i(atoi(value));
		break;

	    case FLOAT:
		callback.f(atof(value));
		break;

	    default:
		// bad valueType, do nothing
		break;
	    }
	}
    };


    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *
     * SerialController
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    #define NORMAL_PROTOCOL 0
    #define STELE_PROTOCOL 1

    template<int PROTOCOL=NORMAL_PROTOCOL, unsigned int MAX_CALLBACKS=16,
	     size_t MAX_KEY_LEN=32, size_t MAX_VAL_LEN=MAX_KEY_LEN>
    class SerialController {
    public:
	void setup(long baudrate=115200) {}
  
	// overloaded callback-adding functions
	void addCallback(const char *key, voidCallback cb) {
	    m_callbacks.add(key, SerialCallback(cb));
	}
	void addCallback(const char *key, stringCallback cb) {
	    m_callbacks.add(key, SerialCallback(cb));
	}
	void addCallback(const char *key, intCallback cb) {
	    m_callbacks.add(key, SerialCallback(cb));
	}
	void addCallback(const char *key, floatCallback cb) {
	    m_callbacks.add(key, SerialCallback(cb));
	}
  
	// message sending
	void sendMessage(const char *messageKey, const char *messageValue) {}
	void sendMessage(const char *messageKey) {}
	void sendMessage(const char *messageKey, int messageValue) {}
	void sendMessage(const char *messageKey, float messageValue) {}


	// updating
	void update() {}


	// size info
	size_t maxKeyLength() { return MAX_KEY_LEN; }
	size_t maxValueLength() { return MAX_VAL_LEN; }
	unsigned int maxNumCallbacks() { return MAX_CALLBACKS; }

	unsigned int numCallbacks() { return m_callbacks.size(); }

    private:
	smm::LookupTable<MAX_CALLBACKS, SerialCallback> m_callbacks;
	smm::FixedSizeString<MAX_KEY_LEN> m_key;
	smm::FixedSizeString<MAX_VAL_LEN> m_value;
    };
}
