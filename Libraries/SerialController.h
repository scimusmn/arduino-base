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
  String messageName;
  enum {
    STRING,
    INT,
    FLOAT
  } valueType;
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


  void (*callback)(char* messageKey, char* messageValue);
  String key, value;
  bool steleProtocol;

  void waitForSerial(long baudrate);

  void cleanString(String& string);

  
public:
  bool handshake;

  SerialManager() { state = WAIT_FOR_START; callback = NULL; }
  void setup(long baudrate,
	     void (*callback_)(char*, char*),
	     bool steleProtocol=true);
  
  void sendMessage(String messageKey, String messageValue);

  void sendMessage(String messageKey, int messageValue);

  void sendMessage(String messageKey, long int messageValue);

  void sendMessage(String messageKey, double messageValue);

  void update();
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialController public member functions
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

void SerialController::setup(long baudrate,
			     void (*callback_)(char*, char*),
			     bool steleProtocol) {
  this->steleProtocol = steleProtocol;
  waitForSerial(baudrate);
  callback = callback_;
}


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


void SerialController::sendMessage(String messageKey, long int messageValue) {
  sendMessage(messageKey, String(messageValue));
}


void SerialController::sendMessage(String messageKey, double messageValue) {
  sendMessage(messageKey, String(messageValue));
}


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
	(*callback)(key.c_str(),value.c_str());
	state = WAIT_FOR_START;
      }
      else {
	key.concat(c);
      }
      break;

    case PARSE_VALUE:
      if (c == '}') {
	(*callback)(key.c_str(),value.c_str());
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


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * SerialController private member functions
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

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

#endif
