# Arduino Base
The goal of this repo is to serve as a go-to boilerplate for quick exhibit prototyping.  It holds SMM's most up-to-date Arduino-related libraries, and a boilerplate sketch for use to quickly build exhibit prototypes. The way you choose to use this repo is up to you, but two suggested ways are noted below.

#### Compatibility
This has been tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud.

#### Please note:
This repo is **NOT** a place to store exhibit-specific libraries or sketches. It is intended only to hold code that is designed to be general and used in a modular way. From time to time, as complexity grows, new libraries will be added and old libraries may be rewritten to run more efficiently or to extend new features. If you do make pull-requests, **please** be sure to keep backwards-compatibility at the forefront of your changes.

---

## Suggested Usage
### Checkout/Clone Method
When you are in the beginning stages of exhibit prototyping, the project you're working on may or may not have an app repo to hold the relevant code, or the exhibit may have no other code. In this case, you can simply clone the repo to your computer and begin modifying the `arduino-base.ino` file to your needs. **DO NOT** commit this file back up this repo as that would be considered app-specific code. Instead, keep the sketch around until you're ready for it become permanent and push it to a new repo.

### Submodule Method
When there is an app repo that will ultimately hold the Arduino sketch you'll be building, it is **strongly** recommended to use this method instead of cloning a copy to the App-specific repo to avoid duplicating non-maintained code. However, in using this method, it is **VERY** important to not modify your local submodule copy of `arduino-base`, because your app-specific repo will the conflict with `arduino-base` and not be able to pull fresh changes.

#### App setup
Generally, inside of your App-specific repo, there will be a directory named `src` located in the repo root directory. We need to create a directory to hold all of our Arduino-related code.

Inside `src`, create a directory named `Arduino`, case-specific like this:
```
$ cd app-repo/src/
$ mkdir Arduino
```

#### Adding the submodule
Inside the directory you just created, you'll need to add the `arduino-base` repo as a submodule by running:
```
$ cd app-repo/src/Arduino/
$ git submodule add https://github.com/scimusmn/arduino-base
```

After that, you'll need to "init" the submodule. To do this, you need to go to the app-specific repo's root and running:
```
$ cd app-repo/
$ git submodule update --init
```

You will know it worked if the console spits out something that looks like:
```
Submodule path 'src/Arduino/arduino-base': checked out '793210fcdf47c8ee42b6caf61d53633727c9fdb9'
```

If the console does not return a message like that you can add the `--force` flag to the command which should definitely return a message.
```
$ cd app-repo/
$ git submodule update --init --force
```

#### Copying the sketch
After getting the submodule path setup, copy the `arduino-base.ino` file from the `arduino-base` submodule to your `Arduino` directory and rename the new copy to `Arduino.ino`. Use this file as a starting point to start working on your project. Inside the sketch are comments that explain how to use the libraries and how you can use the serial messaging system for parsing and sending messages.

---

## Libraries

### AnalogInput.h
This library is primarily a tool used to stop the constant output of Analog Input sensors. It achieves this by averaging `N` number of samples, using the `Average.h` library.

To setup a new `AnalogInput`, you'll need to pass in four parameters:

1. `int` pin, location of the input pin
2. `boolean` enableAverager, to enable averaging via `Averager.h`
3. `boolean` enableLowPass, to enable lowpass filtering via `Averager.h`
4. `void (*callback)(int)`, a callback function that accepts the returned `int` value of the sensor

```
#include "Libraries/AnalogInput.h"

AnalogInput analogInput1;
int analogInput1Pin = A0;

void setup() {

  analogInput1.setup(analogInput1Pin, enableAverager, enableLowPass, [](int analogInputValue) {
    // Do something with analogInputValue
  });

}

void loop() {
  analogInput1.idle();
}
```

### Button.h
This library is used to help [debounce](https://en.wikipedia.org/wiki/Switch#Contact_bounce) digital input signals.

To setup a new `Button`, you'll need to pass in two parameters:

1. `int` pin, location of the input pin
2. `void (*callback)(int)`, a callback function that accepts the returned `int` value of the sensor

```
#include "Libraries/Button.h"

Button button1;
int button1Pin = 2;

void setup() {

  button1.setup(button1Pin, [](int state) {
    if (state == 1) {
      // This means, our button was pressed
      // We should do something, like turn on a light
    }
  });

}

void loop() {
  button1.idle();
}
```

### SerialManager.h
This library is used to handle sending outgoing serial data, via `SerialMessenger.h`, and parsing incoming serial data, via `SerialParser.h`. It serves as a link between [stele](https://github.com/scimusmn/stele) and exhibit-specific apps by sending and receiving JSON messages from [stele](https://github.com/scimusmn/stele).

If you need/choose to use this library in your app, make sure to run the `manager.setup()` function before any other setup function to ensure anything dependent on `SerialManager.h` is available.

To setup a new `SerialManager`, you'll need to pass in two parameters:

1. `long`, the serial [baud rate](https://en.wikipedia.org/wiki/Serial_port#Speed)
2. `void (*callback)(String, int)`, a callback function that accepts a `String` and an `int`.

```
#include "Libraries/SerialManager.h"

SerialManager serialManager;

long baudRate = 115200;

void setup() {

  manager.setup(baudRate, [[](String message, int value) {
    onParse(message, value);
  });

}

void onParse(String message, int value) {
  // This runs when the Arduino receives serial data
}

void loop() {
  serialManager.idle();
}
```

#### Serial Messaging
To send a serial message from the Arduino to the computer/stele, you can use the `sendJsonMessage` function like this:
```
// Arduino sends
manager.sendJsonMessage("some-message-type", 99);

...

// Computer/stele receives
{"message": "some-message-type", "value": 99}

```

#### Serial Parsing
In this example code, data sent and received by the Arduino is formatted in a JSON-style format. To start communication with the Arduino, send a "{" character, without the quotes.

All messages sent to the Arduino from the computer will be echoed back to the computer as verification that the message was correctly received.

---

#### DigitalWrite example:
To turn-on a digital output pin on the Arduino, send from the computer:

```
{"message":"led", "value":1}
```

To turn it off:

```
{"message":"led", "value":0}
```

#### AnalogRead example:
To read an analog pin on the Arduino, send from the computer:

```
{"message":"pot-rotation", "value":1}
```

The following message will be sent back to the computer, with a value ranging from 0-1023:

```
{"message":"pot-rotation", "value":566}
```

#### AnalogWrite example:
To write a PWM value to a PWM pin on the Arduino, send the following from the computer with a value ranging from 0-255:

```
{"message":"pwm-output", "value":130}
```

## Acknowledgements
Modified and adapted from [example code](http://forum.arduino.cc/index.php?topic=396450) written by Robin2.
