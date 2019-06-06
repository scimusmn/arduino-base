/*
  Averager
  created 2015 by A. Heidgerken-Greene, Science Museum of MN
*/
#include "arduino.h"

class Averager {
public:
  int numberOfSamples;
  double * sampleTotal;
  int pointer;
  double total;
  double average;

  Averager(int num = 10) {
  // init the averaging variables.
    pointer = total = average = 0;
    numberOfSamples = num;
    sampleTotal = new double[numberOfSamples];
    for (int i = 0; i < numberOfSamples; i++) {
      sampleTotal[i] = 0;
      total += sampleTotal[i];
    }
  }

  double operator()() {
    return average;
  }

  void reset() {
    for(int i = 0; i < numberOfSamples; i++) {
      sampleTotal[i] = 0;
    }

    total = average = 0;
  }

  void idle(double newRead) {
    total -= sampleTotal[pointer];
    sampleTotal[pointer] = newRead;
    total += newRead;
    average = total / numberOfSamples;
    pointer = (pointer + 1) % numberOfSamples;
  }
};
