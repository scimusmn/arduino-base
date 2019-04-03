/* SMM Serial data parser and COM protocol for Arduino I/O pin control, v1
   modified 3/30/2019 from source code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
   by D. Bailey, Science Museum of MN

   To be used with Arduino UNO, or Pro-Mini, and SMM stele/electron-wrapper

********Incoming serial data***************

  Valid string data received by the Arduino is parsed into four parts:
  pin: number, state: of pin (1 or 0), PWM value (set range0-255), retrieve analog value (returned range 0-1023)

  Examples:

  Digital write:

      To turn on digital pin 2, the string data received should be in the following format:

             {digitalPin:2, state:1, pwm:0, getVal:0}

      pwm should always be set to "0". State can either be 1(on) or 0(off)

 
  Analog write:

      To write an analog value of 150 to PWM pin 3, the string data received should be in the following format:

             {digitalPin:3, state:0, pwm:150, getVal:0}

      State and getVal should always be set to "0". PWM range is 0(full off) to 255(full on).
      Use on Arduino pins D3,D5,D6,D9,D10,D111

 
  Analog Read:

       To retrieve an analog value from analog A0 that is reading 5 volts, the string data should be in the following format:

             {analogPin:0, state:0, pwm:0, getVal:0}

       To be used with Arduino pins A0-A5


  The examples above demonstrate sending string data in a highly verbose way, so that it is easier to follow during the front-end
  coding process.  However, the data that the Arduino is actually storing and acting upon is much shorter. For example, to turn
  on digital pin 2, the minimun string data that needs to be sent out is:

     {:2:1:0:0}

  IMPOTANT: The order of the number series in the string matters!

  In the example above, the curley brackets are used to inidcate where to start parsing the data that has come into the receive buffer on the
  Arduino, and where to stop.  The colons serve as parsing delimiters. The number after each colon delimiter is stored into an array, then
  converted to an integer.

  The first number in the series is used to specify the Arduino I/O pin.  The second number is used to set the state of the specified
  digital pin (1-on or 0-off).  The third number is used to set to write a pmw value on the specifed pwm pin (0-255). The fourth number is used
  to direct the Arduino to return an analog value from the specifed analog pin (1-to return the analog value or 0-not used).


*********Outgoing serial data***************

  Outgoing data format to the computer may be determined on a case by case basis

  Examples of typical format:

     When a button is pressed on the Arduino, send string data out to the computer in the following format:
        {message:vrs-button-press, value:true} value can either be true or false

     When reading analog values from a potentiometer or sensor, send string data out to the computer in the following format:
        {message:anythinghere, value:458} value returned can range from 0-1023
*/

//global variables

const byte numChars = 32; // max number of characters the array can hold
char receivedChars[numChars];
char tempChars[numChars];
int pin = 0; //physical I/O pin on Arduino
int State = 0; //1=on, 0=off
int pwmval = 0; //pwm value, range 0-255
int getVal = 0; //1 = return value, 0 = do not return value
boolean newData = false;
bool handshake = false;
long timeout = 0;



//attach Button.h library and create an new instance of "Button" class for each button

#include "Button.h" //this library is used to provide debounce control for pushbuttons
Button button1; //this creates an instance of "Button" class called button1.  Add more buttons as needed.
//Button button2; //this creates an instance of "Button" class called button2.  Add more buttons as needed.

//This function reads incoming string data, and stores the characters bound by the start/end markers into an array

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte index = 0;
  char startMarker = ':'; // starting marker, any characters before the first ":" are ignored
  char endMarker = '}'; //end of data string marker
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[index] = rc;
        index++;
        if (index >= numChars) {
          index = numChars - 1;
        }
      }
      else {
        receivedChars[index] = '\0'; // terminate the string
        recvInProgress = false;
        index = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


//this function tokenizes string data and parses it into three parts, then converts the number text to integers

void parseData() {    // split the string data into three parts
  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(tempChars, ":"); // first part
  pin = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ":"); // second part
  State = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ":"); //third part
  pwmval = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ":"); //forth part
  getVal = atoi(strtokIndx);     // convert to a integer

}

//this function writes digital pin states, pwm values to pwm pins, or returns analog values

void writePinState() {
  if (State == 1) digitalWrite(pin, State);
  else if (State == 0 && getVal != 1) (analogWrite(pin, pwmval));
  else if (State == 0 && pwmval == 0 && getVal == 1) {
    Serial.print("{message:analog:");
    Serial.print(pin);
    Serial.print(", value:");
    Serial.print(analogRead(pin));
    Serial.println("}");
  }
}

//initialize serial, setup digital outputs, and setup buttons

void setup() {

  // Configure digital output pins here.  Analog inputs are already configured by default.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);


  // Setup button inputs here
  button1.setup(7, [](int state) {  //number after the "(" refers to the pin number the button is attached to
    if (state) Serial.println("{message:vrs-button-press, value:true}");  //put string data message between quotes;
  });
  //button2.setup(10, [](int state){  //number after the "(" refers to the pin number the button is attached to
  //  if (state) Serial.println("{message:anyname, value:true}");  //put string data message between quotes;
  //});

  Serial.begin(9600); //set serial baud rate to 9600
  timeout = millis(); //get current time
  while (!Serial); //wait for serial port to open
  while (!handshake) { //loop here until valid handshake data has been sent by computer
    if (Serial.available() > 0 && Serial.read() == '{') { //if data is available, and first character is a "{", send message to computer
      Serial.println("{message:Arduino-ready, value:true}");
      handshake = true;
    }
    if (millis() > timeout + 50) {  //time-out and clear input buffer if no valid data sent
      while (Serial.available()) Serial.read();
      timeout = millis(); //get current time
    }
  }
}

//main Loop

void loop() {
  button1.idle(); //watch for a button press
  //button2.idle(); //watch for a button press
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars); // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the colons with \0
    parseData();
    writePinState();
    newData = false;
  }

  /*

    For future use

   *******analog reads into Arduino, analog read values sent out in constant stream*******

    int A0val = analogRead(A0); //read analog value from pin A0, store into variable A0val
    Serial.print("{message:anythinghere, value:");
    Serial.print(A0val);
    Serial.println("}");
  */
}



