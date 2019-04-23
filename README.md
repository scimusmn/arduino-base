# ArduinoSerialParser
Simple method for controlling and reading Arduino I/O pins by sending and receiving string data via a computer COM port

SMM Serial data parser and COM protocol for Arduino I/O pin control.  Modified and adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450

Tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud

In this example code, data sent and recieved by the Arduino is formatted in a JSON-style format  
   
To start communication with the Arduino, send a "{" character, less the quotes

All messages sent to the Arduino from the computer will be echoed back to the computer as verification that the
message was correctly received
 

DigitalWrite example: 
  
   To turn-on a digital output pin on the Arduino, send from the computer:
   
      {"message":"led", "value":1}
    
   To turn it off:
      
       {"message":"led", "value":0}
  

AnalogRead example:
  
   To read an analog pin on the Arduino, send from the computer:
   
      {"message":"pot-rotation", "value":1}
    
The following message will be sent back to the computer, with a value ranging from 0-1023:
      
      {"message":"pot-rotation", "value":566}
  

AnalogWrite example:
    
   To write a PWM value to a PWM pin on the Arduino, send the following from the computer with a
   value ranging from 0-255:
   
      {"message":"pwm-output", "value":130}  
