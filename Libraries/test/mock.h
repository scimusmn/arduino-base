#pragma once

#define CHANGE 0

int digitalPinToInterrupt(int pin);
void attachInterrupt(int k, void (*isr)(), int type);

unsigned long millis();

int digitalRead(int pin);

class Serial_ {
	public:
	void begin(unsigned long) {}
	void print(char *str) {}
	void print(const char *str) {}
	void println(char *str) {}
	void println(const char *str) {}
	int available() { return 0; }
	char read() { return 0; }
} extern Serial;
