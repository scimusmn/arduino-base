#pragma once

#ifdef UNIT_TEST
#include "tests/FakeSerial.h"
#else
#include <SoftwareSerial.h>
#endif

#include "Tag.h"
#include "FixedSizeString.h"


namespace smm {
    typedef void (*tagReadCallback)(RfidTag&);
    
    class ID12LA {
    public:
	void setup(int rxPin, tagReadCallback callback) {
	    serial = new SoftwareSerial(rxPin, 3);
	    serial->begin(9600);
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
	    serial->println(str.c_str());
	    unsigned char d0 = getDigit(0);
	    unsigned char d1 = getDigit(1);
	    unsigned char d2 = getDigit(2);
	    unsigned char d3 = getDigit(3);
	    unsigned char d4 = getDigit(4);
	    unsigned char checksum = getDigit(5);
	    RfidTag tag(d0, d1, d2, d3, d4);
	    if (tag.checksum() == checksum)
		callback(tag);
	}

	int getDigit(int index) {
	    char c0 = str.c_str()[2*index];
	    char c1 = str.c_str()[(2*index) + 1];
	    return (16*hexDigit(c0)) + hexDigit(c1);
	}

	int hexDigit(char c) {
	    switch (c) {
	    case '0':
		return 0;
	    case '1':
		return 1;
	    case '2':
		return 2;
	    case '3':
		return 3;
	    case '4':
		return 4;
	    case '5':
		return 5;
	    case '6':
		return 6;
	    case '7':
		return 7;
	    case '8':
		return 8;
	    case '9':
		return 9;
	    case 'a':
	    case 'A':
		return 10;
	    case 'b':
	    case 'B':
		return 11;
	    case 'c':
	    case 'C':
		return 12;
	    case 'd':
	    case 'D':
		return 13;
	    case 'e':
	    case 'E':
		return 14;
	    case 'f':
	    case 'F':
		return 15;
	    default:
		return 0;
	    }
	}
    };
}
