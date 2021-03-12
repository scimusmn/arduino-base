#ifndef SERIAL_CONTROLLER_HPP
#define SERIAL_CONTROLLER_HPP

#define MAX_STRING_LEN 128

#include <Arduino.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef enum {
  WAIT_FOR_START,
  PARSE_KEY,
  PARSE_KEY_OVERFLOW,
  PARSE_VALUE,
  PARSE_VALUE_OVERFLOW,
  N_PARSE_STATES
} parseState;

static void cleanString(char* string);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class SerialController {
  private:
    parseState state;
    void (*callback)(char* messageKey, char* messageValue);
    char key[MAX_STRING_LEN];
    char value[MAX_STRING_LEN];
    int keyIndex, valueIndex;

  public:
    bool handshake;

  SerialManager() { state = WAIT_FOR_START; callback = NULL; }
  void setup(long baudrate, void (*callback_)(char*, char*)) {
    waitForSerial(baudrate);
    callback = callback_;
  }

  void waitForSerial(long baudrate) {
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

  void sendMessage(char* messageKey, char* messageValue) {
    cleanString(messageKey);
    cleanString(messageValue);

    char result[2*MAX_STRING_LEN+3];
    strcpy(result,"{");
    strcat(result,messageKey);
    strcat(result,":");
    strcat(result,messageValue);
    strcat(result,"}");

    Serial.println(result);
  }

  void sendMessage(char* messageKey, int messageValue) {
    char stringValue[MAX_STRING_LEN];
    snprintf(stringValue, MAX_STRING_LEN, "%d", messageValue);
    sendMessage(messageKey, stringValue);
  }

  void sendMessage(char* messageKey, unsigned int messageValue) {
    char stringValue[MAX_STRING_LEN];
    snprintf(stringValue, MAX_STRING_LEN, "%u", messageValue);
    sendMessage(messageKey, stringValue);
  }

  void sendMessage(char* messageKey, long int messageValue) {
    char stringValue[MAX_STRING_LEN];
    snprintf(stringValue, MAX_STRING_LEN, "%ld", messageValue);
    sendMessage(messageKey, stringValue);
  }

  void sendMessage(char* messageKey, long unsigned int messageValue) {
    char stringValue[MAX_STRING_LEN];
    snprintf(stringValue, MAX_STRING_LEN, "%lu", messageValue);
    sendMessage(messageKey, stringValue);
  }

  void sendMessage(char* messageKey, float messageValue) {
    char stringValue[MAX_STRING_LEN];
    snprintf(stringValue, MAX_STRING_LEN, "%f", messageValue);
    sendMessage(messageKey, stringValue);
  }

  void update() {
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
        if (keyIndex == MAX_STRING_LEN-1) {
          key[keyIndex] = 0;
          state = PARSE_KEY_OVERFLOW;
        }
        else if (c == '{') {
          strcpy(key,"");
          strcpy(value,"");
          keyIndex = 0;
          valueIndex = 0;
        }
        else if (c == ':') {
          key[keyIndex] = 0;
          state = PARSE_VALUE;
        }
        else if (c == '}') {
          key[keyIndex] = 0;
          (*callback)(key,value);
          state = WAIT_FOR_START;
        }
        else {
          key[keyIndex] = c;
          keyIndex++;
        }
        break;

      case PARSE_VALUE:
        if (valueIndex == MAX_STRING_LEN - 1) {
          value[valueIndex] = 0;
          state = PARSE_VALUE_OVERFLOW;
        }
        else if (c == '}') {
          value[valueIndex] = 0;
          (*callback)(key,value);
          state = WAIT_FOR_START;
        }
        else {
          value[valueIndex] = c;
          valueIndex++;
        }
        break;

      case PARSE_KEY_OVERFLOW:
        if (c == ':') {
          state = PARSE_VALUE;
        }
        else if (c == '{') {
          strcpy(key,"");
          strcpy(value,"");
          keyIndex = 0;
          valueIndex = 0;
          state = PARSE_KEY;
        }
        break;

      case PARSE_VALUE_OVERFLOW:
        if (c == '{') {
          strcpy(key,"");
          strcpy(value,"");
          keyIndex = 0;
          valueIndex = 0;
          state = PARSE_KEY;
        }
        else if (c == '}') {
          (*callback)(key,value);
          state = WAIT_FOR_START;
        }
        break;

      default:
        // something's gone wrong, reset
        state = WAIT_FOR_START;
        break;
      }
    }
  }
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

#endif
