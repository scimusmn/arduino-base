#include "arduino.h"

#include "SerialMessenger.h"
#include "SerialParser.h"

class SerialManager {
private:
  SerialMessenger messenger;
  SerialParser parser;
public:
  // Handshake initial state
  boolean handshake = false;

  void (*parserCallback)(String message, int value);

  SerialManager() {}

  void setup(long baudRate, void (*CB)(String, int)) {

    waitForSerial(baudRate);

    messenger.setup();
    parserCallback = CB;
    parser.setup(parserCallback);
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
        messenger.sendJsonMessage("Arduino-ready", 1);
      }
    }
  }

  void idle() {
    parser.idle();
  }
};
