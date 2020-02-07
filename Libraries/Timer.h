#include "Arduino.h"

// ensure this library description is only included once
#ifndef Timer_h
#define Timer_h

class Timer {
private:
  unsigned long currentDuration;
  unsigned long duration;
  boolean ended = false;
  boolean postponed = false;
  unsigned long postponedDuration;
  boolean running = false;
  unsigned long timeElapsed = 0;
  unsigned long timeStarted = 0;
public:
  void (*callback)(boolean, boolean, unsigned long);

  Timer() {}

  void setup(void (*CB)(boolean, boolean, unsigned long), unsigned long timerDuration) {
    callback = CB;
    duration = timerDuration;
  }

  void clear() {
    if (running == true) {
      ended = false;
      postponed = false;
      running = false;
      timeStarted = 0;
      timeElapsed = 0;
    }
  }

  boolean isRunning() {
    return running;
  }

  void postpone(unsigned long postponeAmount) {
    postponed = true;
    postponedDuration = timeElapsed + postponeAmount;
  }

  void start() {
    if (running == false) {
      ended = false;
      postponed = false;
      postponedDuration = duration;
      running = true;
      timeStarted = millis();
      timeElapsed = 0;
    }
  }

  void update() {
    if (running == true) {

      // Let's increment anything that needs it
      unsigned long now = millis();
      timeElapsed = now - timeStarted;

      currentDuration = (postponed == false) ? duration : postponedDuration;
      if (now >= (timeStarted + currentDuration)) {
        running = false;
        ended = true;
      }
    }
    else {
      ended = false;
    }

    callback(running, ended, timeElapsed);
  }
};

#endif
