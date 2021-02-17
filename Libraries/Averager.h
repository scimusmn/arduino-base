#include "Arduino.h"

// ensure this library description is only included once
#ifndef Averager_h
#define Averager_h

class Averager {
 public:
  Averager(int requestedNumberOfSamples = 10) {
    samplingTotal = 0;
    numberOfSamples = requestedNumberOfSamples;
    sampleReadings = new int[numberOfSamples];

    // Setup/populate our measurements array with values of 0
    for (int i = 0; i < numberOfSamples; i++) {
      sampleReadings[i] = 0;
    }
  }
  int calculateAverage() { return samplingTotal / numberOfSamples; }

  void insertNewSample(double newSample) {
    samplingTotal -= sampleReadings[samplePointer];
    sampleReadings[samplePointer] = newSample;
    samplingTotal += newSample;
    samplePointer = (samplePointer + 1) % numberOfSamples;
  }

 private:
  int numberOfSamples;
  int* sampleReadings;
  int samplePointer;
  long samplingTotal;
};

#endif
