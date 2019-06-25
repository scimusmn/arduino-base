#include "arduino.h"

#include "ArduinoJson-v6.11.1.h"
#include "SerialMessenger.h"
#include "SerialParser.h"

class SerialManager {
private:
  SerialMessenger messenger;
  SerialParser parser;
public:
  boolean handshake = false;

  void (*parserCallback)(char* message, int value);

  SerialManager() {}

  void setup(long baudRate, void (*CB)(String, int), boolean parserDebug) {

    waitForSerial(baudRate);

    parserCallback = CB;
    parser.setup(parserCallback, parserDebug);
  }

  void sendJsonMessage(String message, int value) {
    messenger.sendJsonMessage(message, value);
  }

  void waitForSerial(long baudRate) {
    // Set serial baud rate
    Serial.begin(baudRate);

    // Wait for serial port to open
    while (!Serial);

    // Wait here until { character is received
    while (!handshake) {

      // Send confirmation message to computer
      if (Serial.available() > 0 && Serial.read() == '{') {
        handshake = true;
        messenger.sendJsonMessage("arduino-ready", 1);
        Serial.read();
      }
    }
  }

  void idle() {
    parser.idle();
  }
};
