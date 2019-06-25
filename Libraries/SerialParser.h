#include "arduino.h"
#include "ArduinoJson-v6.11.1.h"

class SerialParser {
public:
  void (*callback)(char* message, int value);
  boolean sendDebugMessages = false;

  SerialParser() {}

  void setup(void (*CB)(char*, int), boolean debug) {
    callback = CB;
    sendDebugMessages = debug;
  }

  void idle() {
    while (Serial.available() > 0) {
      StaticJsonDocument<250> doc;
      DeserializationError err = deserializeJson(doc, Serial);

      if (err) {
        if (sendDebugMessages == true) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(err.c_str());
        }
      }
      else {
        callback(doc["message"], doc["value"]);
      }
    }
  }
};
