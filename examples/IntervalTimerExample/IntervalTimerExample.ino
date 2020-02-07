#include "IntervalTmr.h"

IntervalTmr intTimer1;

void setup() {
 intTimer1 = IntervalTmr(3, 1000, 500); //pin number, on time, off time
}
void loop() {
  intTimer1.enable();
}
