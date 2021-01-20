#include "Arduino.h"

// ensure this library description is only included once
#ifndef Button_h
#define Button_h

class Button
{
private:
  int pin;
  bool lastPinState;
  unsigned long pinChangeMillis;

public:
  bool buttonState;
  int debounce;
  void (*callback)(int);

  Button(int p, void (*CB)(int)) //pin, callback
  {
    callback = CB;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    debounce = 20;
    lastPinState = buttonState = (digitalRead(pin));
  }

  void update()
  {
    bool pinState = digitalRead(pin);

    //if the state of the pin has changed.
    if (pinState != lastPinState)
    {
      lastPinState = pinState;
      pinChangeMillis = millis();
    }

    if (((millis() - pinChangeMillis) > debounce) && buttonState != pinState)
    {
      buttonState = pinState;
      callback(!buttonState);
    }
  }
};

#endif
