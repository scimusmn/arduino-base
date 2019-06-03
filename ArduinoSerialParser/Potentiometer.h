#include "arduino.h"

class Potentiometer {
public:
  unsigned long debounceTimer;
  int debounce;
  int pin;
  void (*callback)(int );

  int potentiometerValue;
  int numReadings;
  int readings[10];
  int readingIndex;
  long total;
  int average;

  Potentiometer() {}

  void setup(int p, void (*CB)(int), unsigned long time = 20) {
    callback = CB;
    pin = p;
    debounceTimer = 0;
    debounce = time;
    potentiometerValue = analogRead(pin);

    numReadings = 10;

    for (int thisReading = 0; thisReading < numReadings; thisReading++)
      readings[thisReading] = 0;

    total = 0;
    average = 0;
  }

  void idle() {
    int currentPotentiometerValue = analogRead(pin);

    // Subtract the last reading:
    total = total - readings[readingIndex];

    // Read from the sensor:
    readings[readingIndex] = currentPotentiometerValue;

    // Add the reading to the total:
    total = total + readings[readingIndex];

    // Advance to the next position in the array
    readingIndex = readingIndex + 1;

    // Reset index
    if (readingIndex >= numReadings) readingIndex = 0;

    // Calculate the average:
    average = total / numReadings;

    if (average != potentiometerValue) {
      potentiometerValue = average;
      debounceTimer = millis() + debounce;
    }

    if (debounceTimer < millis()) {
      callback(potentiometerValue);
    }
  }
};
