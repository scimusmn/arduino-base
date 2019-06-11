// Serial Data Parser and COM protocol for Arduino I/O pin control
// Modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
// Originally authored on 4/22/2019 by D. Bailey, Science Museum of Minnesota

#include "Libraries/AnalogInput.h"
#include "Libraries/Button.h"
#include "Libraries/SerialMessenger.h"

// Arduino digital output pin assignments
#define led 3
#define pwm_output 5

int analogInputPin1 = A0;
int buttonPin = 2;

// Variables used by parser
const byte numChars = 64; // Buffer array size
char receivedChars[numChars];
char tempChars[numChars]; // Temporary array for use when parsing
char messageIn[numChars] = {0};
char functionIn[numChars] = {0};
char valueIn[numChars] = {0};
int intval = 0;
String message;
String function;
String value;
boolean newData = false;

// Handshake initial state
boolean handshake = false;

AnalogInput analogInput1;
Button button1;

SerialMessenger messenger;

void setup() {

  // We need to setup our SerialMessenger
  messenger.setup();

  // For every sketch, we need to set up our IO
  // By default, we're setting up one digital input and one analog input

  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(led, OUTPUT);
  pinMode(pwm_output, OUTPUT);

  // ANALOG INPUTS

  // Parameter 1: pin location
  // Parameter 2: enable averaging to get a less constant stream of data
  boolean enableAverager = true;
  // Parameter 3: enable lowpass filter for Averager to further smooth value
  boolean enableLowPass = true;
  // Parameter 4: callback

  analogInput1.setup(analogInputPin1, enableAverager, enableLowPass, [](int analogInputValue) {
    messenger.sendJsonMessage("analog-input1", analogInputValue);
  });

  // DIGITAL INPUTS

  // Parameter 1: pin location
  // Parameter 2: callback

  button1.setup(buttonPin, [](int state) {
    if (state) messenger.sendJsonMessage("button1-press", 1);
  });

  // Set serial baud rate
  Serial.begin(115200);

  // Wait for serial port to open
  while (!Serial);

  // Wait here until { character is received
  while (!handshake) {
    if (Serial.available() > 0 && Serial.read() == '{') {

      // Send confirmation message to computer

      handshake = true;
      messenger.sendJsonMessage("Arduino-ready", 1);
    }

    if (Serial.read() != '{') {
      // Clear serial input buffer if character is not valid
      while (Serial.available()) Serial.read();
    }
  }
}

// Setup data starting and ending markers
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '{'; // Start of data string marker
  char endMarker = '}';  // End of data string marker
  char rc;

  // When serial data is available, read characters in until the end marker is reached
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // Terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

// Parse the received data by splitting the string data into four parts
void parseData() {
  char * strtokIndx; // This is used by strtok() as an index

  strtokIndx = strtok(tempChars, ":"); // Parse the "message" keyword in the json data
  strcpy(messageIn, strtokIndx);       // Copy the characters to messageIn

  strtokIndx = strtok(NULL, ","); // Parse the "function" keyword in the json data, (i.e. "led")
  strcpy(functionIn, strtokIndx); // Copy the characters functionIn array

  strtokIndx = strtok(NULL, ":"); // Parse the "value" keyword in the json data
  strcpy(valueIn, strtokIndx);    // Copy it to valueIn array

  strtokIndx = strtok(NULL, ":"); // Parse the number character in the json data
  intval = atoi(strtokIndx);      // Convert the number character to an integer, store it in intval

  messageIn[10] = '\0'; // Max number of characters set to 10
  message = messageIn; // Convert characters in messageIn array to a string

  functionIn[40] = '\0'; // Max number of characters set to 40
  function = functionIn; // Convert characters in functionIn array to a string

  valueIn[10] = '\0'; // Max number of characters set to 10
  value = valueIn; // Convert characters in valueIn array to a string
}

// Write pin states and send out confirmation and analog values over serial
void writePins() {
  if (message == "\"message\"" && function == "\"led\"" && value == " \"value\""  && intval == 1) {
    // Turn-on led
    digitalWrite(led, intval);
  }
  else if (message == "\"message\"" && function == "\"led\"" && value == " \"value\""  && intval == 0) {
    // Turn-off led
    digitalWrite(led, intval);
  }
  else if (message == "\"message\"" && function == "\"pwm-output\"" && value == " \"value\"" && intval >= 0) {
    // Set pwm value to pwm pin
    Serial.print(intval);
    Serial.println("}");
    analogWrite(pwm_output, intval);
  }
  else if (message == "\"message\"" && function == "\"pot-rotation\"" && value == " \"value\"" && intval == 1) {
    messenger.sendJsonMessage("pot-rotation", analogInput1.readValue());
  }
  else {
    messenger.sendJsonMessage("unknown-command", 1);
  }
}

// Main loop
void loop() {
  analogInput1.idle();
  button1.idle();

  recvWithStartEndMarkers();
  if (newData == true) {
    // This temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the colons and commas with a null character
    strcpy(tempChars, receivedChars);

    parseData();
    writePins();

    newData = false;
  }
}
