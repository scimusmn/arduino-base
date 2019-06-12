// Averager
// Originally authored in 2015 by A. Heidgerken-Greene, Science Museum of Minnesota

#include "arduino.h"

class Averager {
public:

  // Setup variables
  int pin;
  int numberOfSamples;
  boolean useLowpassFilter = false;

  int * sampleReadings;
  int currentSample;
  long samplingTotal;

  Averager() {}

  void setup(int analogInput, int requestedNumberOfSamples = 10, boolean useLowpass = false) {
    pin = analogInput;

    // Tells us if we should use a lowpass filter on sensor readings
    useLowpassFilter = useLowpass;

    samplingTotal = 0;
    numberOfSamples = requestedNumberOfSamples;
    sampleReadings = new int[numberOfSamples];

    // Setup/populate our measurements array with values of 0
    for (int i = 0; i < numberOfSamples; i++) {
      sampleReadings[i] = 0;
    }
  }

  // This actually does pin reads and calculates the average
  int calculateAverage() {
    for (int i = 0; i < numberOfSamples; i++) {
      int newSample = analogRead(pin);

      if (useLowpassFilter == true) {
        // Simple Low Pass filter to smooth out the jitter
        newSample = (0.8 * newSample) + ((1 - 0.8) * currentSample);
      }

      // Update our samplingTotal with the new measurement
      samplingTotal -= sampleReadings[i];
      sampleReadings[i] = newSample;
      samplingTotal += sampleReadings[i];
    }

    currentSample = samplingTotal / numberOfSamples;
    return currentSample;
  }
};
