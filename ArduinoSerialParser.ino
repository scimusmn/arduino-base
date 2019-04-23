
/* SMM Serial data parser and COM protocol for Arduino I/O pin control, v2
   modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
   4/22/2019
   by D. Bailey, Science Museum of Minnesota

   Tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud

   In this example code, data sent and recieved by the Arduino is formatted in a JSON-style format:  
   
   To start communication with the Arduino, send a { character.

   DigitalWrite example: 
  
    To turn-on a digital output pin on the Arduino, send from the computer:
   
      {"message":"led", "value":1}

    To turn it off:

      {"message":"led", "value":0}

   AnalogRead example:

    To read an analog pin on the Arduino, send from the computer:
   
      {"message":"pot-rotation", "value":1}

    The following message will be sent out to the computer, with a value ranging from 0-1023:
      
      {"message":"pot-rotation", "value":566}

  AnalogWrite example:

    To write a pwm value to a pwn pin on the Arduino, send the following from the computer with a
    value ranging from 0-255:
   
       {"message":"pwm-output", "value":130}  
       
*/

// Arduino digital output pin assignments

int led = 3;
int pwm_output = 5;

// Arduino analog input pin assignments

int pot = 0;

const byte numChars = 64;// buffer array size
char receivedChars[numChars];
char tempChars[numChars];  // temporary array for use when parsing

// variables to hold the parsed data
char messageIn[numChars] = {0};
char functionIn[numChars] = {0};
char valueIn[numChars] = {0};
int val = 0;
boolean newData = false;
bool handshake = false;
long timeout = 0;

// attach Button.h library and create an new instance of "Button" class for each button

#include "Button.h" //this library is used to provide debounce control for pushbuttons

Button button1; //this creates an instance of "Button" class called button1.  Add more buttons as needed.
Button button2; //this creates an instance of "Button" class called button2.  Add more buttons as needed.


// setup digital outputs and button inputs, attach pins to button functions, and initialize serial

void setup() {

  pinMode(led, OUTPUT);
  pinMode(pwm_output, OUTPUT);

  // setup button inputs here, and associated actions when buttons are pressed

  button1.setup(2, [](int state) {  //number after the "(" refers to the pin number the button is attached to
    if (state) Serial.println("{\"message\":\"vrs-button-press\", \"value\":true}");  //put string data message between quotes;
  });
  button2.setup(4, [](int state) { //number after the "(" refers to the pin number the button is attached to
    if (state) Serial.println("{\"message\":\"door-opened\", \"value\":true}");  //put string data message between quotes;
  });


  // setup serial and wait for handshake with computer

  Serial.begin(115200); //set serial baud rate
  timeout = millis(); //get current time
  while (!Serial); //wait for serial port to open
  while (!handshake) { //loop here until valid handshake data has been sent by computer
    if (Serial.available() > 0 && Serial.read() == '{') { //if data is available, and first character is a {, send message to computer
      Serial.println("{\"message\":\"Arduino-ready\", \"value\":true}");
      handshake = true;
    }
    if (millis() > timeout + 50) {  //time-out and clear input buffer if no valid data sent
      while (Serial.available()) Serial.read();
      timeout = millis(); //get current time
    }
  }
}

// read serial data when available

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '{'; //start of data string marker
  char endMarker = '}';  //end of data string marker
  char rc;

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
        receivedChars[ndx] = '\0'; // terminate the string
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

// parse incoming serial data

void parseData() {      // split the string data into four parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ":"); // get the first part - the string
  strcpy(messageIn, strtokIndx);       // copy it to messageIn

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  strcpy(functionIn, strtokIndx); // get the second string and copy it to functionIn

  strtokIndx = strtok(NULL, ":"); // this continues where the previous call left off
  strcpy(valueIn, strtokIndx);    // get the third string, and copy it to valueIn

  strtokIndx = strtok(NULL, ":"); // this continues where the previous call left off
  val = atoi(strtokIndx);         // convert string "val" to an integer

}

// write pin states and send out confirmation and analog values over serial

void writePins() {

  messageIn[20] = '\0';
  String message = messageIn; //convert char array to string
  functionIn[40] = '\0';
  String function = functionIn; //convert char array to string
  valueIn[20] = '\0';
  String value = valueIn; //convert char array to string

  // Setup messages to be recieved and acted upon here

  if (message == "\"message\"" && function == "\"LED\"" && value == " \"value\""  && val == 1) {
    Serial.println("{\"message\":\"led\", \"value\":true}");
    digitalWrite(led, val);
  }
  else if (message == "\"message\"" && function == "\"LED\"" && value == " \"value\""  && val == 0) {
    Serial.println ("{\"message\":\"led\", \"value\":false}");
    digitalWrite(led, val);
  }
  else if (message == "\"message\"" && function == "\"pwm-output\"" && value == " \"value\"" && val >= 0) {
    Serial.print("{\"message\":\"pwm-output\", \"value\":");
    Serial.print(val);
    Serial.println("}");
    analogWrite(pwm_output, val);
  }
  else if (message == "\"message\"" && function == "\"pot-rotation\"" && value == " \"value\"" && val == 1) {
    Serial.print("{\"message\":\"pot-rotation\", \"value\":");
    Serial.print(analogRead(pot));
    Serial.println("}");
  }
  else {
    Serial.println("Unknown command");
  }
}

// main loop

void loop() {
  button1.idle();
  button2.idle();
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    writePins();
    newData = false;
  }
}




