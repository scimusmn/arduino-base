#include "arduino-base/Libraries/AnalogInput.h"

AnalogInput analogInput1;
int analogInput1Pin = 1;

void setup() {
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
}

void loop() {
  analogInput1.update();
}
