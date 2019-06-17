#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/SerialManager.h"

SerialManager serialManager;

long baudRate = 115200;

AnalogInput analogInput1;
Button button1;

// Pin assignments
#define analogInput1Pin A0
#define button1Pin 2
#define ledPin 3
#define pwmOutputPin 5

void setup() {

  // Ensure Serial Port is open and ready to communicate
  serialManager.setup(baudRate, [](String message, int value) {
    onParse(message, value);
  });

  // For every sketch, we need to set up our IO
  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(ledPin, OUTPUT);
  pinMode(pwmOutputPin, OUTPUT);

  // ANALOG INPUTS

  // Parameter 1: pin location
  // Parameter 2: enable averaging to get a less constant stream of data
  boolean enableAverager = false;
  // Parameter 3: the number of samples to average
  int averagerSampleRate = 10;
  // Parameter 4: enable lowpass filter for Averager to further smooth value
  boolean enableLowPass = false;
  // Parameter 5: callback

  analogInput1.setup(analogInput1Pin, enableAverager, averagerSampleRate, enableLowPass, [](int analogInputValue) {
    serialManager.sendJsonMessage("analog-input1", analogInputValue);
  });

  // DIGITAL INPUTS

  // Parameter 1: pin location
  // Parameter 2: callback

  button1.setup(button1Pin, [](int state) {
    if (state) serialManager.sendJsonMessage("button1-press", 1);
  });
}

void loop() {
  analogInput1.idle();
  button1.idle();
  serialManager.idle();
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
    serialManager.sendJsonMessage("pwm-set", value);
  }
  else if (message == "\"pot-rotation\"" && value == 1) {
    serialManager.sendJsonMessage("pot-rotation", analogInput1.readValue());
  }
  else {
    serialManager.sendJsonMessage("unknown-command", 1);
  }
}
