#pragma once

#ifdef UNIT_TEST
#include "tests/FakeArduino.h"
#include "tests/FakeWire.h"
#include "tests/FakeEEPROM.h"
#else
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#endif

#include "ID12LA.h"
#include "Tag.h"


namespace smm {
    class RfidReader {
    public:
	static const int 
	void setup() {}

    private:
	ID12LA id12la;
	RfidTag tag;
    };
}
