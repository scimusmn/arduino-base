#include "arduino-base/Libraries/SerialController.hpp"

SerialController serialController;

long baudrate = 115200;
int blinkrate = 1000;
bool blinking = true;
bool led_on = false;
long blinkcounter = 0;

#define ledpin 13

void setup() {

  // Enables/disables debug messaging from ArduinoJson
  boolean arduinoJsonDebug = false;

  // Ensure Serial Port is open and ready to communicate
  serialController.setup(baudrate, &onParse);

  // For every sketch, we need to set up our IO
  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(ledpin, OUTPUT);
}

void loop() {
  // update SerialController and check for new data
  serialController.update();

  if (blinking) {
    if ((millis() - blinkcounter) >= blinkrate) {
      led_on = !led_on;
      digitalWrite(ledpin,led_on);
      blinkcounter = millis();

      // put led_on into a string via snprintf()
      char ledOn[5];
      snprintf(ledOn,5,"%d",led_on);

      // send data to stele. note that they must both be strings!
      serialController.sendMessage("led-status", ledOn);
    }
  }
}

// this function will run when serialController reads new data
void onParse(char* message, char* value) {
  if (strcmp(message, "led") == 0) {
    // Turn-on led
    digitalWrite(ledpin, atoi(value));
  }
  else if (strcmp(message, "blinkrate") == 0) {
    // set blinkrate
    // NOTE: atoi() converts a string containing a number to an integer.
    //       use atof() if you want a float
    blinkrate = atoi(value);
  }
  else if (strcmp(message, "blinking") == 0) {
    // turn blinking on or off
    blinking = atoi(value);
  }
  else if (strcmp(message, "wake-arduino") == 0 && strcmp(value, "1") == 0) {
    // you must respond to this message, or else
    // stele will believe it has lost connection to the arduino
    serialController.sendMessage("arduino-ready", "1");
  }
  else {
    // helpfully alert us if we've sent something wrong :)
    serialController.sendMessage("unknown-command", "1");
  }
}
