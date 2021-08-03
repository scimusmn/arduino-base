#include "FakeArduino.h"

unsigned long millis_timer = 0;
std::map<int, FakePin> arduino;

bool digitalRead(int pin) {
    return arduino[pin].digitalValue;
}

void digitalWrite(int pin, bool value) {
    arduino[pin].digitalValue = value;
}

void pinMode(int pin, int mode) {
    arduino[pin].mode = mode;
}

unsigned int analogRead(int pin) {
    return arduino[pin].analogValue;
}

void analogWrite(int pin, unsigned int value) {
    arduino[pin].analogValue = value;
}

void delay(unsigned long ms) {
    millis_timer += ms;
}

unsigned long millis() {
    return millis_timer;
}
