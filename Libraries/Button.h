#include "Arduino.h"

class Button {
public:
  bool state;
  bool fired;
  bool lastFired;
  unsigned long debounceTimer;
  int debounce;
  int pin;
  void (*callback)(int state);

  Button() {}

  void setup(int p, void (*CB)(int)) {
    callback = CB;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    debounceTimer = 0;
    debounce = 20;
    lastFired = state = fired = true;
  }

  void idle() {
    if (digitalRead(pin) != state) {
      state = !state;
      fired = !state;
      debounceTimer = millis() + debounce;
    }

    if (debounceTimer < millis() && state != fired && lastFired != state) {
      lastFired = fired = state;
      callback(!state);
    }
  }
};
