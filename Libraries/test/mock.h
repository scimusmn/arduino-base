#pragma once

#define CHANGE 0

int digitalPinToInterrupt(int pin);
void attachInterrupt(int k, void (*isr)(), int type);

unsigned long millis();

int digitalRead(int pin);
