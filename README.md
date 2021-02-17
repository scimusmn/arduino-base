# Arduino Base
The goal of this repo is to serve as a go-to boilerplate for rapid Arduino prototyping. It holds SMM's most up-to-date Arduino-related libraries, and a boilerplate sketch for use to quickly build exhibit prototypes. The way you choose to use this repo is up to you, but two suggested ways are noted below.

### Compatibility
This has been tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud.

### Please note:
This repo is **NOT** a place to store exhibit-specific libraries or sketches. It is intended only to hold code that is designed to be general and used in a modular way. From time to time, as complexity grows, new libraries will be added and old libraries may be rewritten to run more efficiently or to extend new features. If you do make pull-requests, **please** be sure to keep backwards-compatibility at the forefront of your changes.

## Prerequisites
In order to verify your sketch code compiles and to burn it to the Arduino, you'll first need to [download](https://www.arduino.cc/en/Main/Software) the official Arduino IDE and drivers. Instructions on the usage of the Arduino IDE can be found [here](https://www.arduino.cc/en/Guide/Environment).

## Suggested Usage with Stele
There are two suggested way to incorporate this repo into stele-compatible projects. If you need to do rapid prototyping, an app-specific repository does not exist, or you just want to get familiar with writing Arduino code, you're probably better off doing the Checkout/Clone method.

If an app-specific repo does exist and you are familiar with SMM's repo management style, you're better off using the Submodule method. This method is a little bit more advanced, you should know about [`git submodules`](https://git-scm.com/book/en/v2/Git-Tools-Submodules) before diving in, but the relevant commands are included to get you started.

### Checkout/Clone Method
When you are in the beginning stages of exhibit prototyping, the project you're working on may or may not have an app repo to hold the relevant code, or the exhibit may have no other code. If this is the case, or as mentioned above, you just want to start working on Arduino code, this method is preferred.

To clone the repo to your machine either run this command via a command line:

```
$ git clone https://github.com/scimusmn/arduino-base
```

or click the "[Download Button](https://github.com/scimusmn/arduino-base/archive/master.zip)" on the GitHub page and save as a Zip file to your computer.

**NOTE**: if you save the repo as a Zip file, you'll need to rename the unzipped directory to `arduino-base`, case-specific, or the Arduino IDE will complain during compile time and not compile.

After cloning the repo to your computer, you can begin modifying the `arduino-base.ino` file to suit your needs. **DO NOT** commit this file back up this repo as that would be considered app-specific code. Instead, keep the sketch around until you're ready for it become permanent and then incorporate it into an app-specific repo.

### Submodule Method
When there is an app repo that will ultimately hold the Arduino sketch you'll be building, it is **strongly** recommended to use this method instead of cloning a copy to the App-specific repo to avoid duplicating non-maintained code. However, in using this method, it is **VERY** important to **NOT** modify your local submodule copy of `arduino-base`, because your app-specific repo will the conflict with `arduino-base` and not be able to pull fresh changes.

To see what an app setup with this method looks like, [stel-duino](https://github.com/scimusmn/stel-duino) is a good example.

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

#### Setting up an app
Finally, after following the setup above, you'll need to register an `ipcRenderer` from `Electron` to allow stele and `arduino-base` to communicate. 


##### create-react-app
If you're using the SMM `create-react-app`, this can be accomplished by adding a reference in `public/index.html` file.
```html
<body>
  ...

  <!-- Expose Electron's IPC Renderer for use within React Components -->
  <script>
    /* eslint-disable */
    if (typeof require !== 'undefined') {
        window.ipcRef = require('electron').ipcRenderer;
    }
  </script>

  ...
</body>
```

##### Gatsby
If you're using a **Gatsby** application, such as SMM's [app-template](https://github.com/scimusmn/app-template), you will need to [customize the default `html.js` file](https://www.gatsbyjs.com/docs/custom-html/) to inject a script that makes the reference.
1. Make a copy of `.cache/default-html.js`. Place in the `src/` folder and rename to `html.js`. 
```bash
cd your-gatsby-repo
cp .cache/default-html.js src/html.js
```
2. Edit `src/html.js` and insert the entire `<script>` tag seen below. It should be the last tag before the `</body>` closing tag. We use React's [dangerouslySetInnerHTML](https://www.gatsbyjs.com/docs/custom-html/#adding-custom-javascript) attribute which should be used with caution, but this is an appropriate exception. 
```html
<body>
  ...
  {props.postBodyComponents}

  <!-- Expose Electron's IPC Renderer for use within React Components -->
  <script
    dangerouslySetInnerHTML={{
      __html: `
        if (typeof require !== 'undefined') {
            window.ipcRef = require('electron').ipcRenderer;
          }
      `,
    }}
  />
</body>
```

#### React example page
An example of a React component can be found at `/ReactSerial/examples/arduino-test-page.js`. 
If you're using Gatsby, simply copy this file into your `pages` directory and rebuild your site. A page will be generated at `http://localhost:8000/arduino-test-page` that can be used to test communication with the IPC Renderer. 
If you aren't using Gatsby, use this file as a reference, but you'll need to update the package pathing to match your own app structure.


## Libraries

### AnalogInput.h
This library is primarily a tool used to stop the constant output of Analog Input sensors. It achieves this by averaging `int averagerSampleRate` number of samples, using the `Average.h` library.

To setup a new `AnalogInput`, you'll need to pass in four parameters:

1. `int` pin, location of the input pin
2. `boolean` enableAverager, to enable averaging via `Averager.h`
3. `int`, the number of samples to average
4. `boolean` enableLowPass, to enable lowpass filtering via `Averager.h`
5. `void (*callback)(int)`, a callback function that accepts the returned `int` value of the sensor

#### AnalogInput Example
```
#include "Libraries/AnalogInput.h"

AnalogInput analogInput1;
int analogInput1Pin = A0;

void setup() {

  analogInput1.setup(analogInput1Pin, enableAverager, averagerSampleRate, enableLowPass, [](int analogInputValue) {
    // Do something with analogInputValue
  });

}

void loop() {
  analogInput1.update();
}
```

### Button.h
This library is used to help [debounce](https://en.wikipedia.org/wiki/Switch#Contact_bounce) digital input signals.

To setup a new `Button`, you'll need to pass in two parameters:

1. `int` pin, location of the input pin
2. `void (*callback)(int)`, a callback function that accepts the returned `int` value of the sensor
3. (optional) `int` debounce value in milliseconds.

#### Button Example
See Examples Folder

### Timer.h
This library is used to handle common timer-related functions. Currently, it does timing by counting from `millis()` although, in the future, `micros()` may be implemented.

To setup a new `Timer`, you'll need to pass in two parameters:

1. `void (*callback)(boolean, boolean, unsigned long)`, a callback function that accepts the returned `int` value of the sensor
2. `unsigned long`, the duration of the timer

#### Timer Methods
| Method   | Arguments                      | Returns | Description                                                      |
| :------- | :----------------------------- | :------ | :--------------------------------------------------------------- |
| postpone | (unsigned long postponeAmount) | void    | Resets the timer's duration to `postponeAmount`, provided in ms. |

#### Timer Example
```
#include "Libraries/Button.h"
#include "Libraries/Timer.h"

Button button1;
Timer timer1;

void setup() {

  timer1.setup([](boolean running, boolean ended, unsigned long timeElapsed) {
    if (running == true) {
      // Let's make sure a light is on!
    }

    if (ended == true) {
      // Time's up! Lights out.
    }

  }, 3000);

  button1.setup(button1Pin, [](int state) {
    if (state == 1) {

      if (timer1.isRunning() == false) {
        timer1.start();
      }
      else {
        timer1.postpone(3000);
      }
    }
  });
}

void loop() {
  timer1.update();
}
```

### SerialManager.h
This library is used to handle sending outgoing serial data, via `SerialMessenger.h`, and parsing incoming serial data, via `SerialParser.h`. It serves as a link between [stele](https://github.com/scimusmn/stele) and exhibit-specific apps by sending and receiving [JSON](https://json.org/) messages from [stele](https://github.com/scimusmn/stele).

If you need/choose to use this library in your app, make sure to run the `manager.setup()` function before any other setup function to ensure anything dependent on `SerialManager.h` is available.

To setup a new `SerialManager`, you'll need to pass in two parameters:

1. `long`, the serial [baud rate](https://en.wikipedia.org/wiki/Serial_port#Speed)
2. `void (*callback)(String, int)`, a callback function that accepts a `String` and an `int`.

#### SerialManager Example
```
#include "Libraries/SerialManager.h"

SerialManager serialManager;

long baudRate = 115200;

void setup() {

  manager.setup(baudRate, &onParse);

}

void onParse(char* message, char* value) {
  // This runs when the Arduino receives serial data
  if (strcmp(message, "specific-message") == 0 && value == 1)
    // Do something
  }
}

void loop() {
  serialManager.update();
}
```

#### Serial Messaging
To send a serial message from the Arduino to the computer/stele, you can use the `sendMessage` function like this:
```
// Arduino sends
manager.sendMessage("some-message-type", 99);

...

// Computer/stele receives
{some-message-type:99}

```

#### Serial Parsing
To start communication with the Arduino, send a "{" character, without the quotes.

All messages sent to the Arduino from the computer will be echoed back to the computer as verification that the message was correctly received.

---

#### DigitalWrite example:
To turn-on a digital output pin on the Arduino, send from the computer:

```
{led:1}
```

To turn it off:

```
{led:0}
```

#### AnalogRead example:
To read an analog pin on the Arduino, send from the computer:

```
{pot-rotation:1}
```

The following message will be sent back to the computer, with a value ranging from 0-1023:

```
{pot-rotation:566}
```

#### AnalogWrite example:
To write a PWM value to a PWM pin on the Arduino, send the following from the computer with a value ranging from 0-255:

```
{pwm-output:130}
```

## Troubleshooting
### Reintializing the submodule
While editing apps, you may run into issues if you make changes to any of the underlying submodule files inside of an app the submodule

1. Delete the relevant section from the `.gitmodules` file
2. Delete the relevant section from `.git/config`
3. Run `git rm --cached path_to_submodule` (no trailing slash)
4. Run `rm -rfi .git/modules/src/Arduino` (no trailing slash)
6. Delete the now untracked submodule files `rm -rfi src/Arduino/arduino-base`

You are now ready to add the submodule again, the steps to follow are listed above.

## Acknowledgements
#### Averager.h
Originally authored in 2015 by [A. Heidgerken-Greene](https://github.com/heidgera)

#### Button.h
Originally authored in 2017 by [A. Heidgerken-Greene](https://github.com/heidgera)

#### SerialManager.h, SerialMessenger.h, and SerialParser.h
Modified and adapted in 2019 by [D. Bailey](https://github.com/twsdbailey), from [example code](http://forum.arduino.cc/index.php?topic=396450) written by Robin2.
