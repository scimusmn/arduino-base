#include "arduino.h"
#include "ArduinoJson-v6.11.1.h"

class SerialParser {
public:
  void (*callback)(char* message, int value);

  SerialParser() {}

  void setup(void (*CB)(char*, int)) {
    callback = CB;
  }

  void idle() {
    while (Serial.available() > 0) {
      StaticJsonDocument<250> doc;
      DeserializationError err = deserializeJson(doc, Serial);

      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
      }
      else {
        callback(doc["message"], doc["value"]);
        Serial.read();
      }
    }
  }
};
