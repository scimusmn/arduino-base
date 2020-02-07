#include "arduino.h"

class IntervalTmr {
  private:
    byte pinNum;
    unsigned long onTime;
    unsigned long offTime;
    unsigned long nextTime = 0;
  public:
    IntervalTmr() {}
    bool tmrState;
    IntervalTmr (byte pinNum, unsigned long onTime, unsigned long offTime) {
      this->pinNum = pinNum;
      this->onTime = onTime;
      this->offTime = offTime;
      pinMode(pinNum, OUTPUT);
    }
    void enable() {
      unsigned long timeNow = millis();
      if (timeNow >= nextTime) {
        if (tmrState) {
          tmrState = false;
          nextTime = timeNow + offTime;
        }
        else {
          tmrState = true;
          nextTime = timeNow + onTime;
        }
        digitalWrite(pinNum, tmrState);
      }
    }
};
