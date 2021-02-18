#include "Arduino.h"

// ensure this library description is only included once
#ifndef Button_h
#define Button_h

class Button {
private:
  int pin;
  bool lastPinState;
  int debounce;
  unsigned long pinChangeMillis;
  bool buttonState;
  void (*callback)(int);

public:
  // constructor (pin, callback function, [optional] debounce in millis)
  Button(int p, void (*CB)(int), int _debounce = 20) {
    callback = CB;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    debounce = _debounce;
    lastPinState = buttonState = (digitalRead(pin));
  }

  // returns the button state
  bool getState() {
    return buttonState;
  }

  //run in a loop when button press should be noticed.
  void update() {
    bool pinState = digitalRead(pin);

    //if the state of the pin has changed.
    if (pinState != lastPinState) {
      lastPinState = pinState;
      pinChangeMillis = millis();
    }

    //if the pin state was stable for the debounce value in millis.
    if (((millis() - pinChangeMillis) > debounce) && buttonState != pinState) {
      buttonState = pinState;
      callback(buttonState);
    }
  }
};

#endif
