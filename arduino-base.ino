#include "Libraries/SerialController.hpp"

SerialController serialController;
long baudRate = 115200;

void setup() {
  // Ensure Serial Port is open and ready to communicate
  serialController.setup(baudRate, &onParse);
}

void loop() {
  serialController.update();
}

void onParse(char* message, char* value) {
  if (strcmp(message, "wake-arduino") == 0 && atoi(value) == 1) {
    serialController.sendMessage("arduino-ready", 1);
  }
  else {
    serialController.sendMessage("unknown-command", 1);
  }
}
