#include "arduino.h"
#include "ArduinoJson-v6.11.1.h"

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
