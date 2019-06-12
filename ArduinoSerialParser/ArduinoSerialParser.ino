// Serial Data Parser and COM protocol for Arduino I/O pin control
// Modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
// Originally authored on 4/22/2019 by D. Bailey, Science Museum of Minnesota

#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/SerialMessenger.h"
#include "Libraries/SerialParser.h"

#include "Constants/AppSketch.h"

// Handshake initial state
boolean handshake = false;

AppSketch app;
SerialMessenger messenger;
SerialParser parser;

void setup() {

  // We need to setup our SerialHelper classes
  messenger.setup();

  app.setup(messenger);

  parser.setup([](String message, int intval) {
    app.writePins(message, intval);
  });

  // Set serial baud rate
  Serial.begin(115200);

  // Wait for serial port to open
  while (!Serial);

  // Wait here until { character is received
  while (!handshake) {

    // Send confirmation message to computer
    if (Serial.available() > 0 && Serial.read() == '{') {
      handshake = true;
      messenger.sendJsonMessage("Arduino-ready", 1);
    }

    // Clear serial input buffer if character is not valid
    if (Serial.read() != '{') {
      while (Serial.available()) Serial.read();
    }
  }
}

// Main loop
void loop() {
  parser.idle();
  app.idle();
}
