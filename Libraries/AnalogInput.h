#include "Arduino.h"
#include "Averager.h"

// ensure this library description is only included once
#ifndef AnalogInput_h
#define AnalogInput_h

class AnalogInput {
public:
  int analogInputPin;
  void (*callback)(int);
  boolean enableAverager;
  boolean enableLowPass;
  int sampleRate = 10;

  int analogInputValue;
  Averager averager;
  int debounce = 20;
  unsigned long debounceTimer = 0;

  AnalogInput() {}

  void setup(int pinLocation, boolean getAverage, int averagerSampleRate, boolean getLowPass, void (*CB)(int)) {
    analogInputPin = pinLocation;
    callback = CB;
    enableAverager = getAverage;
    sampleRate = averagerSampleRate;
    enableLowPass = getLowPass;

    if (enableAverager) {
      averager.setup(sampleRate, enableLowPass);
    }
  }

  void update() {
    if (enableAverager == true) {
      averager.insertNewSample(analogRead(analogInputPin));

      if (debounceTimer < millis()) {
        int average = averager.calculateAverage();

        if (average != analogInputValue) {
          analogInputValue = average;
          callback(analogInputValue);
          debounceTimer = millis() + debounce;
        }
      }
    }
    else {
      analogInputValue = readValue();
      callback(analogInputValue);
    }
  }

  int readValue() {
    int val;
    if (enableAverager){
      val = averager.calculateAverage();
    } else {
      val = analogRead(analogInputPin);
    }

    return val;
  }
};

#endif
