#ifndef SERIAL_CONTROLLER_HPP
#define SERIAL_CONTROLLER_HPP

#define MAX_STRING_LEN 128

#include <Arduino.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef enum {
              WAIT_FOR_START,
              PARSE_KEY,
              PARSE_VALUE,
              N_PARSE_STATES }
  parseState;

static void cleanString(char* string);   

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class SerialController {
 private:
  parseState state;
  void (*callback)(char* messageKey, char* messageValue);
  char key[MAX_STRING_LEN];
  char value[MAX_STRING_LEN];
  int keyIndex, valueIndex;

  void waitForSerial(long baudrate);
  
 public:
  bool handshake;

  SerialManager() { state = WAIT_FOR_START; callback = NULL; }
  void setup(long baudrate, void (*callback_)(char*, char*)) {
    waitForSerial(baudrate);
    callback = callback_;
  }
  
  void sendMessage(char* messageKey, char* messageValue);

  void update();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void cleanString(char* string) {
  // remove '{', '}', and ':' delimiter characters from a string
  char result[MAX_STRING_LEN];
  
  int i = 0;
  int j = 0;
  while(string[i] != 0) {
    if (string[i] == '{'
        || string[i] == '}'
        || string[i] == ':') {
      i++;
    }
    else {
      result[j] = string[i];
      i++; j++;
    }
  }

  // close string
  result[j] = 0;
  
  string = result;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::waitForSerial(long baudrate) {
  Serial.begin(baudrate);
  while(!Serial);
  
  while(!handshake) {
    if (Serial.available() && Serial.read() == '{') {
      handshake = true;
      sendMessage("arduino-ready","1");
    }
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::sendMessage(char* messageKey, char* messageValue) {
  cleanString(messageKey);
  cleanString(messageValue);

  char result[2*MAX_STRING_LEN+3];
  strcpy(result,"{");
  strcat(result,messageKey);
  strcat(result,":");
  strcat(result,messageValue);
  strcat(result,"}");

  Serial.print(result);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SerialController::update() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (state) {
    case WAIT_FOR_START:
      if (c == '{') {
        state = PARSE_KEY;
        strcpy(key,"");
        strcpy(value,"");
        keyIndex = 0;
        valueIndex = 0;
      }
      break;
    case PARSE_KEY:
      if (c == ':') {
        key[keyIndex] = 0;
        state = PARSE_VALUE;
      }
      else {
        key[keyIndex] = c;
        keyIndex++;
      }
      break;
    case PARSE_VALUE:
      if (c == '}') {
        value[valueIndex] = 0;
        (*callback)(key,value);
        state = WAIT_FOR_START;
      }
      else {
        value[valueIndex] = c;
        valueIndex++;
      }
      break;
    default:
      // something's gone wrong, reset
      state = WAIT_FOR_START;
      break;
    }
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
#endif      
