#pragma once

#ifdef UNIT_TEST
#include "../tests/FakeSerial.h"
#else
#include <SoftwareSerial.h>
#endif

#include "Tag.h"
#include "../FixedSizeString.h"


namespace smm {
    typedef void (*tagReadCallback)(RfidTag&);
    
    class ID12LA {
    public:
	void setup(int rxPin, tagReadCallback callback) {
	    serial = new SoftwareSerial(rxPin, 3);
	    this->callback = callback;
	    state = WAIT_STX;
	}
	void update() {
	    while (serial->available())
		eatChar(serial->read());
	}
	#ifdef UNIT_TEST
	SoftwareSerial* getSerial() { return serial; }
	#endif
    private:
	enum { WAIT_STX, READ_TAG, READ_NEWLINE, READ_ETX } state;
	tagReadCallback callback;
	SoftwareSerial *serial;
	String16 str;

	bool charIsOkay(char c) {
	    return c == '0' ||
		c == '1' ||
		c == '2' ||
		c == '3' ||
		c == '4' ||
		c == '5' ||
		c == '6' ||
		c == '7' ||
		c == '8' ||
		c == '9' ||
		c == 'A' ||
		c == 'B' ||
		c == 'C' ||
		c == 'D' ||
		c == 'E' ||
		c == 'F';
	}

	void eatChar(char c) {
	    switch (state) {
	    case WAIT_STX:
		if (c == '\x02')
		    state = READ_TAG;
		break;
		
	    case READ_TAG:
		if (str.length() < 12 && charIsOkay(c))
		    str.append(c);
		else if (str.length() == 12 && c == '\r')
		    state = READ_NEWLINE;
		else
		    reset();
		break;

	    case READ_NEWLINE:
		if (c == '\n')
		    state = READ_ETX;
		else
		    reset();
		break;

	    case READ_ETX:
		if (c == '\x03')
		    readTag();
		reset();
		break;

	    default:
		// should never get here
		reset();
		break;
	    }
	}

	void reset() {
	    state = WAIT_STX;
	    str = "";
	}

	void readTag() {
	    char substr[3];
	    unsigned char d0 = getDigit(0, substr);
	    unsigned char d1 = getDigit(1, substr);
	    unsigned char d2 = getDigit(2, substr);
	    unsigned char d3 = getDigit(3, substr);
	    unsigned char d4 = getDigit(4, substr);
	    unsigned char checksum = getDigit(5, substr);
	    RfidTag tag(d0, d1, d2, d3, d4);
	    if (tag.checksum() == checksum)
		callback(tag);
	}

	int getDigit(int index, char *substr) {
	    substr[0] = str.c_str()[2*index];
	    substr[1] = str.c_str()[(2*index) + 1];
	    return strtol(substr, NULL, 16);
	}
    };
}
