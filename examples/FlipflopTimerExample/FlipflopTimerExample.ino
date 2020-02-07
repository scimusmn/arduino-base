#include "arduino-base/Libraries/FlipflopTimer.h"

FlipflopTimer flipflopTimer1;

void setup() {
  flipflopTimer1.setup([](boolean flipflopValue) {
    digitalWrite(ledPin, flipflopValue);
  }, 1000, 500);
}
void loop() {
  flipflopTimer1.update();
}
