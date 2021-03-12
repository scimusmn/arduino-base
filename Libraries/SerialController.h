#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#define MAX_STRING_LEN 128

#include <Arduino.h>

typedef void (*stringCallback)(String);
typedef void (*intCallback)(int);
typedef void (*floatCallback)(float);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialCallback
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct SerialCallback {
  String key;

  typedef enum {
    STRING,
    INT,
    FLOAT
  } ValueType;

  ValueType valueType;

  union {
    stringCallback s;
    intCallback i;
    floatCallback f;
  } callback;

  void respond(String value);
};


void SerialCallback::respond(String value) {
  switch (valueType) {
  case STRING:
    callback.s(value);
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


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialController
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef SERIALCONTROLLER_MAX_CALLBACKS
#define SERIALCONTROLLER_MAX_CALLBACKS 16
#endif

class SerialController {
private:
  enum {
    WAIT_FOR_START,
    PARSE_KEY,
    PARSE_VALUE,
    N_PARSE_STATES
  } state;


  int numCallbacks;
  SerialCallback callbacks[SERIALCONTROLLER_MAX_CALLBACKS];
  String key, value;
  bool steleProtocol;

  void waitForSerial(long baudrate);

  void cleanString(String& string);

  void lookupAndRunCallback();

public:
  bool handshake;
  bool errorResponse;

  SerialManager() { state = WAIT_FOR_START; numCallbacks = 0; }

  void setup(long baudrate=115200, bool steleProtocol=true);

  void addCallback(String key, stringCallback callback);
  void addCallback(String key, intCallback callback);
  void addCallback(String key, floatCallback callback);
  
  void sendMessage(String messageKey, String messageValue);
  void sendMessage(String messageKey, int messageValue);
  void sendMessage(String messageKey, float messageValue);

  void update();
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::setup(long baudrate, bool steleProtocol) {
  this->steleProtocol = steleProtocol;
  errorResponse = true;
  waitForSerial(baudrate);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::addCallback(String key, stringCallback callback) {
  SerialCallback* cb = callbacks + numCallbacks;

  cb->key = key;
  cb->valueType = SerialCallback::ValueType::STRING;
  cb->callback.s = callback;
  
  numCallbacks += 1;
}


void SerialController::addCallback(String key, intCallback callback) {
  SerialCallback* cb = callbacks + numCallbacks;

  cb->key = key;
  cb->valueType = SerialCallback::ValueType::INT;
  cb->callback.i = callback;

  numCallbacks += 1;
}


void SerialController::addCallback(String key, floatCallback callback) {
  SerialCallback* cb = callbacks + numCallbacks;

  cb->key = key;
  cb->valueType = SerialCallback::ValueType::FLOAT;
  cb->callback.f = callback;

  numCallbacks += 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::sendMessage(String messageKey, String messageValue) {
  cleanString(messageKey);
  cleanString(messageValue);

  String result = "{";
  result.concat(messageKey);
  result.concat(":");
  result.concat(messageValue);
  result.concat("}");

  Serial.println(result);
}


void SerialController::sendMessage(String messageKey, int messageValue) {
  sendMessage(messageKey, String(messageValue));
}


void SerialController::sendMessage(String messageKey, float messageValue) {
  sendMessage(messageKey, String(messageValue));
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::update() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (state) {
    case WAIT_FOR_START:
      if (c == '{') {
	key = "";
	value = "";
	state = PARSE_KEY;
      }
      break;

    case PARSE_KEY:
      if (c == '{') {
	key = "";
	value = "";
	state = PARSE_KEY;
      }
      else if (c == ':') {
	state = PARSE_VALUE;
      }
      else if (c == '}') {
	lookupAndRunCallback();
	state = WAIT_FOR_START;
      }
      else {
	key.concat(c);
      }
      break;

    case PARSE_VALUE:
      if (c == '}') {
	lookupAndRunCallback();
	state = WAIT_FOR_START;
      }
      else {
	value.concat(c);
      }
      break;

    default:
      // something's gone wrong, reset
      state = WAIT_FOR_START;
      break;
    }
  }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::waitForSerial(long baudrate) {
  Serial.begin(baudrate);
  while(!Serial);

  // wait for handshake if using steleProtocol
  while(!handshake && steleProtocol) {
    if (Serial.available() && Serial.read() == '{') {
      handshake = true;
      sendMessage("arduino-ready","1");
    }
  }
}


void SerialController::cleanString(String& string) {
  string.replace("{", "");
  string.replace("}", "");
  string.replace(":", "");
}


void SerialController::lookupAndRunCallback() {
  if (steleProtocol && key == "wake-arduino") {
    sendMessage("arduino-ready", "1");
    return;
  }

  for (int i=0; i<numCallbacks; i++) {
    if (key == callbacks[i].key) {
      callbacks[i].respond(value);
      return;
    }
  }

  if (errorResponse) {
    sendMessage("unknown-command", key);
  }
}

#endif
