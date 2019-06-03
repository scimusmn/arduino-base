/* SMM Serial data parser and COM protocol for Arduino I/O pin control, v2
   modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
   4/22/2019 by D. Bailey, Science Museum of Minnesota
*/

// Arduino digital output pin assignments
#define led 3
#define pwm_output 5

// Arduino analog input pin assignments
int potentiometer = 0;

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

// Other variables
boolean handshake = false;

// Attached libraries
// This library is used to provide debounce control for pushbuttons and to capture button state
#include "Button.h"

// Add buttons here, set associated actions for these buttons in void setup() below
Button button1; // Creates an instance of "Button" class called button1 (or any name)

// Setup button inputs here, and associated actions when buttons are pressed
void setup() {

  // By default, we're attaching a push button to pin 2
  button1.setup(2, [](int state) {
    if (state) Serial.println("{\"message\":\"button-press\", \"value\":1}");
  });

  // Setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(led, OUTPUT);
  pinMode(pwm_output, OUTPUT);

  // Setup serial and wait for handshake with computer
  Serial.begin(115200); //set serial baud rate
  while (!Serial); //wait for serial port to open

  // Wait here until { character is received
  while (!handshake) {
    if (Serial.available() > 0 && Serial.read() == '{') {
      // Send confirmation message to computer
      Serial.println("{\"message\":\"Arduino-ready\", \"value\":1}");
      handshake = true;
    }
    if (Serial.read()!='{') {
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
    // Read current value on analog channel A0
    Serial.print("{\"message\":\"pot-rotation\", \"value\":");
    Serial.print(analogRead(potentiometer));
    Serial.println("}");
  }
  else {
    // Nothing matched
    Serial.print("{\"message\":\"unknown-command\", \"value\":");
  }
}

// Main loop
void loop() {
  button1.idle();
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // This temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the colons and commas with a null character
    parseData();
    writePins();
    newData = false;
  }
}
