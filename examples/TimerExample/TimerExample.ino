#include "arduino-base/Libraries/Timer.h"

Timer timer1;
int ledPin1 = 1;
int ledPin2 = 2;

void setup() {
  timer1.setup([](boolean running, boolean ended, unsigned long timeElapsed) {
    if (running == true) {
      digitalWrite(ledPin1, true);
      digitalWrite(ledPin2, false);
    }

    if (ended == true) {
      digitalWrite(ledPin1, false);
      digitalWrite(ledPin2, true);
    }

  }, 3000);
}

void loop() {
  timer1.update();
}
