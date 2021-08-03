#pragma once

#include <map>

#define OUTPUT 0
#define INPUT 1
#define INPUT_PULLUP 2

#define LOW 0
#define HIGH 1

struct FakePin {
    int mode = OUTPUT;
    bool digitalValue = false;
    unsigned int analogValue = 0;
};

extern std::map<int, FakePin> arduino;
extern unsigned long millis_timer;


bool digitalRead(int pin);
void digitalWrite(int pin, bool value);
void pinMode(int pin, int mode);
unsigned int analogRead(int pin);
void analogWrite(int pin, unsigned int value);
void delay(unsigned long ms);
unsigned long millis();
