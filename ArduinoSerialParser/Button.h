/* Pushbutton debouncer
   created 2017
   by A. Heidgerken-Greene, Science Museum of MN
*/
#include "arduino.h"

class Button {
public:
  bool state;
  bool fired;
  bool lastFired;
  unsigned long debounceTimer;
  int debounce;
  int pin;
  void (*callback)(int state);

  Button(){
  }

  void setup(int p, void (*CB)(int), unsigned long time = 20) {
    callback = CB;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    debounceTimer = 0;
    debounce = time;
    lastFired = state = fired = true;
  }
  
  void idle(){
    if(digitalRead(pin) != state){
      state = !state;
      fired = !state;
      debounceTimer = millis() + debounce;
    }

    if(debounceTimer < millis() && state != fired && lastFired != state){
      lastFired = fired = state;
      callback(!state);
    }
  }
};
