#include "Arduino.h"

// ensure this library description is only included once
#ifndef FlipflopTimer_h
#define FlipflopTimer_h

class FlipflopTimer {
private:
  unsigned long onTime;
  unsigned long offTime;
  unsigned long nextTime = 0;
public:
  void (*callback)(boolean);
  bool flipflopValue;

  FlipflopTimer() {}

  void setup(void (*CB)(bool), unsigned long duration1Time, unsigned long duration2Time) {
    callback = CB;
    onTime = duration1Time;
    offTime = duration2Time;
  }

  void update() {
    unsigned long timeNow = millis();
    if (timeNow >= nextTime) {
      if (flipflopValue) {
        flipflopValue = false;
        nextTime = timeNow + offTime;
      }
      else {
        flipflopValue = true;
        nextTime = timeNow + onTime;
      }

      callback(flipflopValue);
    }
  }
};

#endif
