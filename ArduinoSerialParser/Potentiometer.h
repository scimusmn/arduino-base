#include "arduino.h"
#include "Averager.h"

class Potentiometer {
public:
  Averager averager;
  unsigned long debounceTimer = 0;
  int debounce = 20;
  int potentiometerValue;

  void (*callback)(int);
  int pin;

  // Set this if you'd like the Averager to lowpass filter the analogRead
  // aka a smoother value over a time with less jitter 
  boolean useLowpassFilter = false;

  Potentiometer() {}

  void setup(int pinLocation, void (*CB)(int)) {
    callback = CB;
    pin = pinLocation;
    averager.setup(pin, 200, useLowpassFilter);
  }

  void idle() {
    if (debounceTimer < millis()) {
      int average = averager.calculateAverage();
      if (average != potentiometerValue) {
        potentiometerValue = average;
        callback(potentiometerValue);
        debounceTimer = millis() + debounce;
      }
    }
  }
};
