#include "Arduino.h"

// ensure this library description is only included once
#ifndef Button_h
#define Button_h

class Button
{
private:
  int pin;
  bool reading;
  unsigned long lastReadingChangeMillis;

public:
  bool state;
  int debounce;
  void (*callback)(int state);

  Button() {}

  void setup(int p, void (*CB)(int))
  {
    callback = CB;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    debounce = 20;
    state = (digitalRead(pin));
  }

  void update()
  {
    if (digitalRead(pin) != reading)
    {
      reading = !reading;
      lastReadingChangeMillis = millis();
    }

    if (((millis() - lastReadingChangeMillis) > debounce) && state != reading)
    {
      state = reading;
      callback(!state);
    }
  }
};

#endif
