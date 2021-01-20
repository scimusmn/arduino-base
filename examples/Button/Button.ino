// To get this sketch to compile
// you must update this library with your own absolute path
// Arduino does not support relative paths :(

#include "C:\Users\jmeyer\Documents\Code\arduino-base\Libraries\Button.h"

//declare pin assignments at the top as constant integers
const int buttonPin = 4;

Button myButton(buttonPin, &buttonFunction, 30); // pin, function reference, (optional)debounce in milliseconds.

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  myButton.listener();
}

void buttonFunction(int state)
{
  //replace with code to execute based on button state.
  digitalWrite(LED_BUILTIN, state);
  if (state == 1)
    Serial.println("Button pressed");
}
