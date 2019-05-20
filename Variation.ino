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

//Arduino digital output pin assignments

  int vrs_button_press = 2;
  int buttonEnable = 3;
  int motorRelay = 5;
  int doorLock =  6;
  int SSR = 7;
  int leds = 8;
  
//timer presets
 int blowerontime = 10000; //10 seconds
 int squirrelontime = 15000; //15 seconds

//Arduino analog input pin assignments

//int potentiometer = 0;

//Arduino digital output pin assignments
  int led = 11;
  int pwm_output = 5;

//Arduino analog input pin assignments
  //int potentiometer = 0;

//variables used by parser
  const byte numChars = 64;// buffer array size
  char receivedChars[numChars];
  char tempChars[numChars];  // temporary array for use when parsing
  char messageIn[numChars] = {0};
  char functionIn[numChars] = {0};
  char valueIn[numChars] = {0};
  int intval = 0;
  String message;
  String function;
  String value;;
  boolean newData = false;
  

//other variables
  bool handshake = false;
  long timeNow = 0;
  long timeNow2 = 0;
  long timeout = 0;
  bool toggle = true;
  
//attached libraries

  #include "Button.h" //this library is used to provide debounce control for 
                    //pushbuttons and to capture button state

//add buttons here, set associated actions for these buttons in void setup() below
  Button vistaFlush; //this creates an instance of "Button" class called vistaFlush.  
  Button keySensor; //this creates an instance of "Button" class called keySensor.  


//setup digital outputs and button inputs, attach pins to button functions, and initialize serial

void setup() {

//setup button inputs here, and associated actions when buttons are pressed

  vistaFlush.setup(2, [](int state) {  //number after the "(" refers to the pin number the button is attached to
    if (state) {
      Serial.println("{\"message\":\"vrs-button-press\", \"value\":1}");//put string data message between quotes;
      digitalWrite(buttonEnable, HIGH); //locks out button after pressed

      timeNow = millis();
      digitalWrite(SSR, HIGH);

      if (toggle) {
        digitalWrite(5, HIGH);
        toggle = false;
        //Serial.println("toggle is true");
      }
      else if (!toggle) {
        digitalWrite(5, LOW);
        toggle = true;
        //Serial.println("toggle is false");
      }
    }

  });

  keySensor.setup(4, [](int state) { //number after the "(" refers to the pin number the button is attached to
    if (state) {
      Serial.println("{\"message\":\"squirrel-key-inserted\", \"value\":1}");
      digitalWrite(leds, LOW);
      timeNow2 = millis();
    }
  });

//setup digital pins and default modes as needed, analog inputs are setup by default
  pinMode(motorRelay, OUTPUT);
  pinMode(SSR, OUTPUT);
  pinMode(doorLock, OUTPUT);
  pinMode(buttonEnable, OUTPUT);
  pinMode(leds, OUTPUT);
  //digitalWrite(buttonEnable, HIGH); //used for testing, bypasses unlock command from computer
  digitalWrite(leds, HIGH);
  
//setup serial and wait for handshake with computer
  Serial.begin(115200); //set serial baud rate
  while (!Serial); //wait for serial port to open
  while (!handshake) { //loop here until valid handshake data has been sent by computer
    if (Serial.available() > 0 && Serial.read() == '{') { //if data is available, and first character is a {, send message to computer
      Serial.println("{\"message\":\"Arduino-ready\", \"value\":1}");
      handshake = true;
    }
    if (Serial.read()!='{') { //clear serial input buffer if character is not valid
      while (Serial.available()) Serial.read();
    }
  }
}

//setup data starting and ending markers
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

//parse the received data

void parseData() {      // split the string data into four parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ":"); // parse the "message" keyword in the JSON data
  strcpy(messageIn, strtokIndx);       // copy the characters to messageIn

  strtokIndx = strtok(NULL, ","); // parse the "function" keyword in the JSON data, (i.e. "led")
  strcpy(functionIn, strtokIndx); // copy the characters functionIn array

  strtokIndx = strtok(NULL, ":"); // parse the "value" keyword in the JSON data
  strcpy(valueIn, strtokIndx);    // copy it to valueIn array

  strtokIndx = strtok(NULL, ":"); // parse the number character in the JSON data
  intval = atoi(strtokIndx);         // convert the number character to an integer, store it in val
  
  messageIn[10] = '\0'; //max number of characters set to 10
  message = messageIn; //convert characters in messageIn array to a string

  functionIn[40] = '\0'; //max number of characters set to 40
  function = functionIn; //convert characters in functionIn array to a string
  
  valueIn[20] = '\0'; //max number of characters set to 10
  value = valueIn; //convert characters in valueIn array to a string

}

//write pin states and send out confirmation and analog values over serial
void writePins() {

  if (message == "\"message\"" && function == "\"door-lock\"" && value == " \"value\""  && intval == 0) {
    //Serial.println("{\"message\":\"door-lock\", \"value\":0}");
    digitalWrite(doorLock, 1); //this operation needs to be inverted, so make state explicit instead of using val
  }
  
  else if (message == "\"message\"" && function == "\"door-lock\"" && value == " \"value\""  && intval == 1) {
    //Serial.println ("{\"message\":\"door-lock\", \"value\":1}");
    digitalWrite(doorLock, 0); //this operation needs to be inverted, so make state explicit instead of using val
  }

  else if (message == "\"message\"" && function == "\"vrs-button-lock\"" && value == " \"value\""  && intval == 0) {
    //Serial.println ("{\"message\":\"vrs-button-lock\", \"value\":0}");
     digitalWrite(buttonEnable, intval);
  }

  else if (message == "\"message\"" && function == "\"vista-flush\"" && value == " \"value\""  && intval == 1) {
   // Serial.println ("{\"message\":\"vista-flush\", \"value\":1}");
    digitalWrite(motorRelay, LOW); //retract actuator
    digitalWrite(SSR, HIGH); //turn on fan
    delay(15000);//delay
    digitalWrite(SSR, LOW);
  }


  //else if (message == "\"message\"" && function == "\"pot-rotation\"" && value == " \"value\"" && val == 1) {
   // Serial.print("{\"message\":\"pot-rotation\", \"value\":");
   // Serial.print(analogRead(pot));
    //Serial.println("}");
  //}
  
  else {
    Serial.println("Unknown command");
  }
}

//main loop

void loop() {
  vistaFlush.idle(); //watch for vistor button press
  keySensor.idle(); //watch for squirrel reveal key switch
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    writePins();
    newData = false;
  }
  if (millis() >= timeNow + blowerontime) { //timer for blower on-time
    digitalWrite(SSR, LOW);
  }
  if (millis() >= timeNow2 + squirrelontime) { //timer for squirrel reveal leds on-time
    digitalWrite(leds, HIGH);
  }
}

