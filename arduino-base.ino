#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/FlipflopTimer.h"
#include "Libraries/SerialController.hpp"
#include "Libraries/Timer.h"

SerialController serialController;

long baudRate = 115200;

AnalogInput analogInput1;
Button button1;
FlipflopTimer flipflopTimer1;
Timer timer1;

// Pin assignments
#define analogInput1Pin A0
#define button1Pin 2
#define ledPin 3
#define pwmOutputPin 5

void setup() {

  // Enables/disables debug messaging from ArduinoJson
  boolean arduinoJsonDebug = false;

  // Ensure Serial Port is open and ready to communicate
  serialController.setup(baudRate, &onParse);

  // For every sketch, we need to set up our IO
  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(ledPin, OUTPUT);
  pinMode(pwmOutputPin, OUTPUT);

  // ANALOG INPUTS

  // Parameter 1: pin location
  // Parameter 2: enable averaging to get a less constant stream of data
  boolean enableAverager = true;
  // Parameter 3: the number of samples to average
  int averagerSampleRate = 10;
  // Parameter 4: enable lowpass filter for Averager to further smooth value
  boolean enableLowPass = false;
  // Parameter 5: callback

  analogInput1.setup(analogInput1Pin, enableAverager, averagerSampleRate, enableLowPass, [](int analogInputValue) {
    serialController.sendMessage("analog-input1", analogInputValue);
  });

  // DIGITAL INPUTS

  // Parameter 1: pin location
  // Parameter 2: callback

  button1.setup(button1Pin, [](int state) {
    if (state) {
      serialController.sendMessage("button1-press", 1);

      if (timer1.isRunning() == false) {
        serialController.sendMessage("isTimerRunning", 1);
        timer1.start();
      }
      else {
        serialController.sendMessage("postpone", 1);
        timer1.postpone(3000);
      }
    }
  });

  flipflopTimer1.setup([](boolean flipflopValue) {
    digitalWrite(ledPin, flipflopValue);
  }, 1000, 500);

  timer1.setup([](boolean running, boolean ended, unsigned long timeElapsed) {
    if (running == true) {
      serialController.sendMessage("timeout-running", timeElapsed);
    }

    if (ended == true) {
      serialController.sendMessage("timeout-ended", timeElapsed);
    }

  }, 3000);
}

void loop() {
  analogInput1.update();
  button1.update();
  flipflopTimer1.update();
  serialController.update();
  timer1.update();
}

void onParse(char* message, char* value) {
  if (strcmp(message, "led") == 0) {
    // Turn-on led
    digitalWrite(ledPin, atoi(value));
  }
  else if (strcmp(message, "pwm-output") == 0 && value >= 0) {
    // Set pwm value to pwm pin
    analogWrite(pwmOutputPin, atoi(value));
    serialController.sendMessage("pwm-set", value);
  }
  else if (strcmp(message, "pot-rotation") == 0) {
    serialController.sendMessage(message, analogInput1.readValue());
  }
  else if (strcmp(message, "wake-arduino") == 0 && atoi(value) == 1) {
    serialController.sendMessage("arduino-ready", 1);
  }
  else {
    serialController.sendMessage("unknown-command", 1);
  }
}
