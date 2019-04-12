# ArduinoSerialParser
Simple method for controlling Arduino I/O pins by sending and receiving string data via a computer COM port


 
# Incoming serial data

  Valid string data received by the Arduino is parsed into four parts:
  pin number, state of pin (1 = high, 0 = low), pwm value (range 0-255), current analog input value 
  of specified pin (1 = read input and send message with value, 0 = dont read input and don't send message). 
  
  Examples:
  
  # digital write:
  
  To turn on digital pin 2, the string data received by the Arduino should be in the following format:
      
          {digitalPin:2, state:1, pwm:0, getVal:0}
      
  pwm should be set to "0", state can either be 1(on) or 0(off)
 
 
  
  # analog write:
  
   To write a pwm value of 150 to pin 3, the string data received by the Arduino 
   should be in the following format:
      
             {digitalPin:3, state:0, pwm:150, getVal:0}
     
   state and getVal should be set to "0".  PWM range is 0(full-off) to 255(full-on).
   use on Arduino pins D3,D5,D6,D9,D10,D11
 
  # analog read:
  
  To read an analog input value from analog A0 and send a message back to the computer with the
  value, the string data received by the Arduino should be in the following format:
            
            {analogPin:0, state:0, pwm:0, getVal:1}
       
  getVal should be set to 1, state and pwm should be set to 0.  
  A value of 0-1023 will be returned in a message back to the computer 
  valid Arduino pins, A0-A5  See "Outgoing serial data" section below for more info 
  
  The examples above demonstrate sending string data in a highly verbose way, for easier readability.  
  However, the data that the Arduino is actually acting is much simpler. 
  For example, to turn on digital pin 2, the minimun string data that needs to be sent out is:
      
      {:2:1:0:0}
  
  IMPORTANT: The order of the number series in the string matters!
  
  In the example above, the curley brackets are used to indicate when to start parsing data stored 
  in the array, and when to stop.  The colons serve as parsing delimiters. The number after each colon 
  delimiter is converted to an integer.
  
  The first number in the series is used to specify the Arduino I/O pin.  The second number is used to set the 
  state of the specified digital pin (1 = high or 0 = low).  The third number is used to write a pmw value (0-255) 
  to the specifed pwm pin. The fourth number is used to direct the Arduino to send a message about the current 
  value of a specified analog input, (1 = read analog input and send message with the value, 
  0 = dont read an analog input and don't send a message).
  
  
# Outgoing serial data

  Outgoing data format to the computer may be determined on a case by case basis
  Examples of a typical format:
    
  When a button is pressed on the Arduino, send a message out to the computer in the following format:
        
     {"message":"vrs-button-press", "value":true} value can either be true or false
     
  When reading analog values from an analog pin, send a message out to the computer in the following format:
        
     {"message":"pot-rotation", "value":458} analog value returned can range from 0-1023
