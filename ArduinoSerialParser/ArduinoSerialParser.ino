// Serial Data Parser and COM protocol for Arduino I/O pin control
// Modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
// Originally authored on 4/22/2019 by D. Bailey, Science Museum of Minnesota

#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/SerialMessenger.h"
#include "Libraries/SerialParser.h"

// Digital Input pin assignments
int analogInputPin1 = A0;
int buttonPin = 2;

// Digital Output pin assignments
#define led 3
#define pwm_output 5

// Handshake initial state
boolean handshake = false;

AnalogInput analogInput1;
Button button1;

SerialMessenger messenger;
SerialParser parser;

void setup() {

  // We need to setup our SerialHelper classes
  messenger.setup();
  parser.setup([](String message, int intval) {
    writePins(message, intval);
  });

  // For every sketch, we need to set up our IO
  // By default, we're setting up one digital input and one analog input

  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(led, OUTPUT);
  pinMode(pwm_output, OUTPUT);

  // ANALOG INPUTS

  // Parameter 1: pin location
  // Parameter 2: enable averaging to get a less constant stream of data
  boolean enableAverager = true;
  // Parameter 3: enable lowpass filter for Averager to further smooth value
  boolean enableLowPass = true;
  // Parameter 4: callback

  analogInput1.setup(analogInputPin1, enableAverager, enableLowPass, [](int analogInputValue) {
    messenger.sendJsonMessage("analog-input1", analogInputValue);
  });

  // DIGITAL INPUTS

  // Parameter 1: pin location
  // Parameter 2: callback

  button1.setup(buttonPin, [](int state) {
    if (state) messenger.sendJsonMessage("button1-press", 1);
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

// Write pin states and send out confirmation and analog values over serial
void writePins(String message, int intval) {
  if (message == "\"led\"" && intval == 1) {
    // Turn-on led
    digitalWrite(led, intval);
  }
  else if (message == "\"led\"" && intval == 0) {
    // Turn-off led
    digitalWrite(led, intval);
  }
  else if (message == "\"pwm-output\"" && intval >= 0) {
    // Set pwm value to pwm pin
    analogWrite(pwm_output, intval);
    messenger.sendJsonMessage("pwm-set", intval);
  }
  else if (message == "\"pot-rotation\"" && intval == 1) {
    messenger.sendJsonMessage("pot-rotation", analogInput1.readValue());
  }
  else {
    messenger.sendJsonMessage("unknown-command", 1);
  }
}

// Main loop
void loop() {
  analogInput1.idle();
  button1.idle();
  parser.idle();
}
