#include "arduino.h"

class Timer {
public:
  void (*callback)(boolean, boolean, unsigned long);
  unsigned long duration;
  boolean ended = false;
  boolean running = false;
  unsigned long timeElapsed = 0;
  unsigned long timeStarted = 0;

  Timer() {}

  void setup(void (*CB)(boolean, boolean, unsigned long), unsigned long timerDuration) {
    callback = CB;
    duration = timerDuration;
  }

  void clear() {
    if (running == true) {
      running = false;
      ended = false;
      timeStarted = 0;
      timeElapsed = 0;
    }
  }

  void start() {
    if (running == false) {
      ended = false;
      running = true;
      timeStarted = millis();
      timeElapsed = 0;
    }
  }

  void idle() {
    if (running == true) {

      // Let's increment anything that needs it
      unsigned long now = millis();
      timeElapsed = now - timeStarted;

      if (now >= (timeStarted + duration)) {
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
