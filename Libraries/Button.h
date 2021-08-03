#pragma once

#ifdef UNIT_TEST
#include "tests/FakeArduino.h"
#else
#include <Arduino.h>
#endif

namespace smm {
    typedef void (*buttonCallback)();
    const int BUTTON_UP = 1;
    const int BUTTON_DOWN = 0;
    

    class Button {
    public:
	// constructor (pin, [optional] debounce in millis)
	Button(int pin, int debounce = 20) :
	    m_pin(pin), m_debounce(debounce), m_state(BUTTON_UP),
	    m_debouncing(false), m_lastState(BUTTON_UP),
	    m_onPress(nullptr), m_onRelease(nullptr),
	    m_whileDown(nullptr), m_whileUp(nullptr)
	{
	    pinMode(m_pin, INPUT_PULLUP);
	}

	// returns the button state
	int getState() { return m_state; }

	// set the callbacks
	void onPress(buttonCallback callback)   { m_onPress = callback; }
	void onRelease(buttonCallback callback) { m_onRelease = callback; }
	void whileDown(buttonCallback callback) { m_whileDown = callback; }
	void whileUp(buttonCallback callback)   { m_whileUp = callback; }

	//run in a loop when button press should be noticed.
	void update() {
	    bool state = digitalRead(m_pin);

	    //if the state of the pin has changed.
	    if (state != m_lastState) {
		m_lastState = state;
		m_debouncing = true;
		m_debounceEndTime = millis() + m_debounce;
	    }

	    //if the pin state was stable for the debounce value in millis.
	    if (m_debouncing && (millis() >= m_debounceEndTime)) {
		m_debouncing = false;
		if (m_state != state) { // state changed, trigger callback
		    m_state = state;
		    if (m_state == BUTTON_UP && m_onRelease != nullptr)
			m_onRelease();
		    else if (m_state == BUTTON_DOWN && m_onPress != nullptr)
			m_onPress();
		}
	    }

	    if (m_state == BUTTON_DOWN && m_whileDown != nullptr)
		m_whileDown();
	    else if (m_state == BUTTON_UP && m_whileUp != nullptr)
		m_whileUp();
	}
	
    private:
	int m_pin;
	int m_debounce;
	int m_state;
	
	bool m_debouncing;
	bool m_lastState;
	unsigned long m_debounceEndTime;
	
	buttonCallback m_onPress;
	buttonCallback m_onRelease;
	buttonCallback m_whileDown;
	buttonCallback m_whileUp;
    };
}
