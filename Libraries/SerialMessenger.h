#include "Arduino.h"
#include "ArduinoJson-v6.11.1.h"

// ensure this library description is only included once
#ifndef SerialMessenger_h
#define SerialMessenger_h

class SerialMessenger {
public:
  SerialMessenger() {}

  void sendJsonMessage(String message, int value) {
    StaticJsonDocument<250> doc;
    JsonObject data = doc.to<JsonObject>();

    data["message"] = message;
    data["value"] = value;

    String serializedJson;
    serializeJson(data, serializedJson);

    Serial.println(serializedJson);
  }
};

#endif
