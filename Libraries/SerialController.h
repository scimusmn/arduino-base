#pragma once

#ifdef UNIT_TEST
#include "tests/FakeSerial.h"
#else
#include <Arduino.h>
#endif

#include "FixedSizeString.h"
#include "LookupTable.h"

#ifndef MAX_KEY_LEN
#define MAX_KEY_LEN 32
#endif

#ifndef MAX_VAL_LEN
#define MAX_VAL_LEN 32
#endif

#define MAX_MSG_LEN (MAX_KEY_LEN + MAX_VAL_LEN + 3)

typedef void (*voidCallback)();
typedef void (*stringCallback)(char*);
typedef void (*intCallback)(int);
typedef void (*floatCallback)(float);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialCallback
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

template<size_t MAX_STR_LEN>
struct SerialCallback {
    SerialCallback(voidCallback cb)   { valueType = VOID;   callback.v = cb; }
    SerialCallback(stringCallback cb) { valueType = STRING; callback.s = cb; }
    SerialCallback(intCallback cb)    { valueType = INT;    callback.i = cb; }
    SerialCallback(floatCallback cb)  { valueType = FLOAT;  callback.f = cb; }

    typedef enum {
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

    void operator()(smm::FixedSizeString<MAX_STR_LEN>& value) {
	switch (valueType) {
	case VOID:
	    callback.v();
	    break;
      
	case STRING:
	    callback.s(value.c_str());
	    break;

	case INT:
	    callback.i(value.toInt());
	    break;

	case FLOAT:
	    callback.f(value.toFloat());
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

#ifndef SERIALCONTROLLER_MAX_CALLBACKS
#define SERIALCONTROLLER_MAX_CALLBACKS 16
#endif

// helper to define repetitious addCallback overloads
#define addCallbackDef(cbType, valType, u)			\
    void addCallback(const char *keyString, cbType callback) {	\
	SerialCallback *cb = callbacks + numCallbacks;		\
	cb->key.str = keyString;				\
	cb->valueType = SerialCallback::ValueType::valType;	\
	cb->callback.u = callback;				\
	numCallbacks++;						\
    }

class SerialController {
private:
  
public:
    bool handshake;
    bool errorResponse;

    void setup(long baudrate=115200, bool steleProtocol=true) {}
  
    // overloaded callback-adding functions
    /*addCallbackDef(voidCallback, VOID, v)
    addCallbackDef(stringCallback, STRING, s)
    addCallbackDef(intCallback, INT, i)
    addCallbackDef(floatCallback, FLOAT, f)*/

  
    // message sending
    void sendMessage(char *messageKey, char *messageValue) {}

    void sendMessage(char *messageKey) {}

    void sendMessage(char *messageKey, int messageValue) {}


    void sendMessage(char *messageKey, float messageValue) {}


    // updating
    void update() {}
};
