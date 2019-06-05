#include "arduino.h"

class Potentiometer {
public:
  unsigned long debounceTimer = 0;
  int debounce = 20;
  int pin;
  void (*callback)(int);

  int potentiometerValue;
  
  int numReadings = 200;
  int readings[200];
  long averageTotal = 0;
  int runningAverage = 0;

  Potentiometer() {}

  void setup(int p, void (*CB)(int)) {
    callback = CB;
    pin = p;
    potentiometerValue = analogRead(pin);

    for (int i = 0; i < numReadings; i++)
      readings[i] = 0;
  }

  void idle() {
    boolean valueChanged = false;

    if (debounceTimer < millis()) {
      for (int i = 0; i < numReadings; i++) {
        int currentPotentiometerValue = (0.8 * analogRead(pin)) + ((1 - 0.8) * potentiometerValue);

        // Update our averageTotal with the new measurment
        averageTotal -= readings[i];
        readings[i] = currentPotentiometerValue;
        averageTotal += readings[i];
      }

      runningAverage = averageTotal / numReadings;

      if (runningAverage != potentiometerValue) {
        potentiometerValue = runningAverage;
        callback(potentiometerValue);
        debounceTimer = millis() + debounce;
      }
    }
  }
};
