#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mock.h"

#define SMM_IMPLEMENTATION
#include "../smm.h"


#include <map>


class Arduino {
	protected:
	std::map<int, int> m_pin;
	std::map<int, void (*)()> m_interrupt;

	unsigned long m_time;

	public:
	Arduino() {
		m_time = 0;
	}

	int digitalRead(int i) {
		if (m_pin.find(i) == m_pin.end()) {
			m_pin[i] = 0;
		}

		return m_pin[i];
	}

	void attachInterrupt(int k, void (*isr)()) {
		m_interrupt[k] = isr;
	}

	unsigned long millis() {
		return m_time;
	}
} arduino;


int digitalPinToInterrupt(int pin) {
	return pin;
}


void attachInterrupt(int k, void (*isr)(), int type) {
	arduino.attachInterrupt(k, isr);
}


unsigned long millis() {
	return arduino.millis();
}


int digitalRead(int pin) {
	return arduino.digitalRead(pin);
}

Serial_ Serial;
