#include "Arduino.h"

// ensure this library description is only included once
#ifndef Averager_h
#define Averager_h

class Averager {
public:

  // Setup variables
  int numberOfSamples;
  boolean useLowpassFilter = false;

  int * sampleReadings;
  int samplePointer;
  long samplingTotal;

  Averager() {}

  void setup(int requestedNumberOfSamples = 10, boolean useLowpass = false) {

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

  int calculateAverage() {
    return samplingTotal / numberOfSamples;
  }

  void insertNewSample(double newSample) {

    if (useLowpassFilter == true) {
      // Simple Low Pass filter to smooth out the jitter
      newSample = (0.8 * newSample) + ((1 - 0.8) * newSample);
    }

    samplingTotal -= sampleReadings[samplePointer];
    sampleReadings[samplePointer] = newSample;
    samplingTotal += newSample;
    samplePointer = (samplePointer + 1) % numberOfSamples;
  }
};

#endif
