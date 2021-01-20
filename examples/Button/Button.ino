// To get this sketch to compile
// you must update this library with your own absolute path
// Arduino does not support relative paths :(

#include "C:\Users\jmeyer\Documents\Code\arduino-base\Libraries\Button.h"

int button1Pin = 2;
Button button1(button1Pin, [](int state) {
  digitalWrite(LED_BUILTIN, state);
});

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  button1.update();
}
