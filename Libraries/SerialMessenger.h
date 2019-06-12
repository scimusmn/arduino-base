#include "arduino.h"

class SerialMessenger {
public:
  SerialMessenger() {}

  void setup() {

  }

  void sendJsonMessage(String message, int value) {
    Serial.print("{\"message\":\"");
    Serial.print(message);
    Serial.print("\", \"value\":");
    Serial.print(value);
    Serial.println("}");
  }
};
