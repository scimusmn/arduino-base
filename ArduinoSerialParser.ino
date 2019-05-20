
/* SMM Serial data parser and COM protocol for Arduino I/O pin control, v2
   modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
   4/22/2019
   by D. Bailey, Science Museum of Minnesota

   Tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud

   In this example code, data sent and recieved by the Arduino is formatted in a JSON-style format used by media
   developers at SMM

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
  int led = 11;
  int pwm_output = 5;

// Arduino analog input pin assignments
  int potentiometer = 0;

// variables used by parser
  const byte numChars = 64;// buffer array size
  char receivedChars[numChars];
  char tempChars[numChars];  // temporary array for use when parsing
  char messageIn[numChars] = {0};
  char functionIn[numChars] = {0};
  char valueIn[numChars] = {0};
  int intval = 0;
  String message;
  String function;
  String value;
  boolean newData = false;

//other variables
  bool handshake = false;

//Attached libraries
  #include "Button.h" //this library is used to provide debounce control for 
                      //pushbuttons and to capture button state

// Add buttons here, set associated actions for these buttons in void setup() below
  Button button1; //creates an instance of "Button" class called button1 (or any name). Add more buttons as needed.
  Button button2; //creates an instance of "Button" class called button2 (or any name). Add more buttons as needed.


void setup() {

//setup button inputs here, and associated actions when buttons are pressed


  button1.setup(2, [](int state) { //button attached to pin 2
    if (state) Serial.println("{\"message\":\"vrs-button-press\", \"value\":1}");  
  });
  button2.setup(4, [](int state) { //button attached to pin 4
    if (state) Serial.println("{\"message\":\"door-opened\", \"value\":true}"); 
  });


//setup digital pins as needed, analog inputs are setup by default
  pinMode(led, OUTPUT);
  pinMode(pwm_output, OUTPUT);


// setup serial and wait for handshake with computer
  Serial.begin(115200); //set serial baud rate
  while (!Serial); //wait for serial port to open
  while (!handshake) { //wait here until { character is received
    if (Serial.available() > 0 && Serial.read() == '{') { 
      Serial.println("{\"message\":\"Arduino-ready\", \"value\":1}"); //send confirmation message to computer
      handshake = true;
    }
    if (Serial.read()!='{') { //clear serial input buffer if character is not valid
      while (Serial.available()) Serial.read();
    }
  }
}

// setup data starting and ending markers
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '{'; //start of data string marker
  char endMarker = '}';  //end of data string marker
  char rc;

//when serial data is available, read characters in until the end marker is reached
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

// parse the received data
void parseData() {      // split the string data into four parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ":"); // parse the "message" keyword in the json data
  strcpy(messageIn, strtokIndx);       // copy the characters to messageIn

  strtokIndx = strtok(NULL, ","); // parse the "function" keyword in the json data, (i.e. "led")
  strcpy(functionIn, strtokIndx); // copy the characters functionIn array

  strtokIndx = strtok(NULL, ":"); // parse the "value" keyword in the json data
  strcpy(valueIn, strtokIndx);    // copy it to valueIn array

  strtokIndx = strtok(NULL, ":"); // parse the number character in the json data
  intval = atoi(strtokIndx);         // convert the number character to an integer, store it in val

  messageIn[10] = '\0'; //max number of characters set to 10
  message = messageIn; //convert characters in messageIn array to a string

  functionIn[40] = '\0'; //max number of characters set to 40
  function = functionIn; //convert characters in functionIn array to a string

  valueIn[10] = '\0'; //max number of characters set to 10
  value = valueIn; //convert characters in valueIn array to a string
}

// write pin states and send out confirmation and analog values over serial
void writePins() {

  //turn-on led
  if (message == "\"message\"" && function == "\"led\"" && value == " \"value\""  && intval == 1) {
    digitalWrite(led, intval);
  }
  //turn-off led
  else if (message == "\"message\"" && function == "\"led\"" && value == " \"value\""  && intval == 0) {
    digitalWrite(led, intval);
  }
  //set pwm value to pwm pin
  else if (message == "\"message\"" && function == "\"pwm-output\"" && value == " \"value\"" && intval >= 0) {
    Serial.print(intval);
    Serial.println("}");
    analogWrite(pwm_output, intval);
  }
  //read current value on analog channel A0
  else if (message == "\"message\"" && function == "\"pot-rotation\"" && value == " \"value\"" && intval == 1) {
    Serial.print("{\"message\":\"pot-rotation\", \"value\":");
    Serial.print(analogRead(potentiometer));
    Serial.println("}");
  }
  //nothing matched
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
    // because strtok() used in parseData() replaces the colons and commas with a null character
    parseData();
    writePins();
    newData = false;
  }
}

