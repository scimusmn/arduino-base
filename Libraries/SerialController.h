#ifndef SERIAL_CONTROLLER_HPP
#define SERIAL_CONTROLLER_HPP

#define MAX_STRING_LEN 128

#include <Arduino.h>

typedef void (*stringResponse)(String);
typedef void (*intResponse)(int);
typedef void (*floatResponse)(float);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialResponse
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct SerialResponse {
  String key;
  typedef enum {
    STRING,
    INT,
    FLOAT
  } ValueType;

  ValueType valueType;
  union {
    stringResponse s;
    intResponse i;
    floatResponse f;
  } response;

  void respond(String value);
};


void SerialResponse::respond(String value) {
  switch (valueType) {
  case STRING:
    response.s(value);
    break;

  case INT:
    response.i(value.toInt());
    break;

  case FLOAT:
    response.f(value.toFloat());
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

// you can #define SERIALCONTROLLER_MAX_RESPONSES before including
// this file to adjust this parameter.
#ifndef SERIALCONTROLLER_MAX_RESPONSES
#define SERIALCONTROLLER_MAX_RESPONSES 16
#endif

class SerialController {
private:
  enum {
    WAIT_FOR_START,
    PARSE_KEY,
    PARSE_KEY_OVERFLOW,
    PARSE_VALUE,
    PARSE_VALUE_OVERFLOW,
    N_PARSE_STATES
  } state;


  int numResponses;
  SerialResponse responses[SERIALCONTROLLER_MAX_RESPONSES];
  String key, value;
  bool steleProtocol;

  void waitForSerial(long baudrate);

  void cleanString(String& string);

  void lookupAndRunCallback();

public:
  bool handshake;

  SerialManager() { state = WAIT_FOR_START; numResponses = 0; }
  void setup(long baudrate=115200, bool steleProtocol=true);

  void addResponse(String key, stringResponse response);
  void addResponse(String key, intResponse response);
  void addResponse(String key, floatResponse response);
  
  void sendMessage(String messageKey, String messageValue);

  void sendMessage(String messageKey, int messageValue);

  void sendMessage(String messageKey, float messageValue);

  void update();
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::setup(long baudrate, bool steleProtocol) {
  this->steleProtocol = steleProtocol;
  waitForSerial(baudrate);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::addResponse(String key, stringResponse response) {
  SerialResponse* r = responses + numResponses;

  r->key = key;
  r->valueType = SerialResponse::ValueType::STRING;
  r->response.s = response;
  
  numResponses += 1;
}


void SerialController::addResponse(String key, intResponse response) {
  SerialResponse* r = responses + numResponses;

  r->key = key;
  r->valueType = SerialResponse::ValueType::INT;
  r->response.i = response;

  numResponses += 1;
}


void SerialController::addResponse(String key, floatResponse response) {
  SerialResponse* r = responses + numResponses;

  r->key = key;
  r->valueType = SerialResponse::ValueType::FLOAT;
  r->response.f = response;

  numResponses += 1;
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

  for (int i=0; i<numResponses; i++) {
    if (key == responses[i].key) {
      responses[i].respond(value);
      return;
    }
  }

  sendMessage("unknown-command", 1);
}

#endif
