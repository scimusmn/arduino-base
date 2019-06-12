#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/SerialManager.h"

SerialManager manager;

long baudRate = 115200;

AnalogInput analogInput1;
Button button1;

// Pin assignments
int analogInputPin1 = A0;
int buttonPin = 2;
int ledPin = 3;
int pwmOutputPin = 5;

void setup() {

  // Ensure Serial Port is open and ready to communicate
  manager.setup(baudRate, [](String message, int value) {
    onParse(message, value);
  });

  // For every sketch, we need to set up our IO
  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(ledPin, OUTPUT);
  pinMode(pwmOutputPin, OUTPUT);

  // ANALOG INPUTS

  // Parameter 1: pin location
  // Parameter 2: enable averaging to get a less constant stream of data
  boolean enableAverager = true;
  // Parameter 3: enable lowpass filter for Averager to further smooth value
  boolean enableLowPass = true;
  // Parameter 4: callback

  analogInput1.setup(analogInputPin1, enableAverager, enableLowPass, [](int analogInputValue) {
    manager.sendJsonMessage("analog-input1", analogInputValue);
  });

  // DIGITAL INPUTS

  // Parameter 1: pin location
  // Parameter 2: callback

  button1.setup(buttonPin, [](int state) {
    if (state) manager.sendJsonMessage("button1-press", 1);
  });
}

void loop() {
  analogInput1.idle();
  button1.idle();
  manager.idle();
}

void onParse(String message, int value) {
  if (message == "\"led\"" && value == 1) {
    // Turn-on led
    digitalWrite(ledPin, value);
  }
  else if (message == "\"led\"" && value == 0) {
    // Turn-off led
    digitalWrite(ledPin, value);
  }
  else if (message == "\"pwm-output\"" && value >= 0) {
    // Set pwm value to pwm pin
    analogWrite(pwmOutputPin, value);
    manager.sendJsonMessage("pwm-set", value);
  }
  else if (message == "\"pot-rotation\"" && value == 1) {
    manager.sendJsonMessage("pot-rotation", analogInput1.readValue());
  }
  else {
    manager.sendJsonMessage("unknown-command", 1);
  }
}
