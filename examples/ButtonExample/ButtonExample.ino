#include "arduino-base/Libraries/Button.h"

Button button1;
int button1Pin = 1;

void setup() {
  // Parameter 1: pin location
  // Parameter 2: callback
  button1.setup(button1Pin, [](int state) {
    if (state) {
      serialController.sendMessage("button1-press", 1);

      if (timer1.isRunning() == false) {
        serialController.sendMessage("isTimerRunning", 1);
        timer1.start();
      }
      else {
        serialController.sendMessage("postpone", 1);
        timer1.postpone(3000);
      }
    }
  });
}

void loop() {
  button1.update();
}
