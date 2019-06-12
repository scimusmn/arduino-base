#include "arduino.h"

class SerialParser {
public:
  const static byte numChars = 64; // Buffer array size
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

  void (*callback)(String function, int intval);

  SerialParser() {}

  void setup(void (*CB)(String, int)) {
    callback = CB;
  }

  void idle() {

    recvWithStartEndMarkers();
    if (newData == true) {
      // This temporary copy is necessary to protect the original data
      // because strtok() used in parseData() replaces the colons and commas with a null character
      strcpy(tempChars, receivedChars);

      parseData();

      if (message == "\"message\"" && value == " \"value\"") {
        callback(function, intval);
      }

      newData = false;
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
};
