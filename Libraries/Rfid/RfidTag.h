#pragma once

#include <cstring>

namespace smm {
    struct RfidTag {
	RfidTag() {}
	RfidTag(unsigned char d0, unsigned char d1, unsigned char d2, unsigned char d3, unsigned char d4) {
	    tagData[0] = d0;
	    tagData[1] = d1;
	    tagData[2] = d2;
	    tagData[3] = d3;
	    tagData[4] = d4;
	}
	unsigned char tagData[5];
	friend bool operator==(RfidTag& lhs, RfidTag& rhs) {
	    return lhs[0] == rhs[0] &&
	    lhs[1] == rhs[1] &&
	    lhs[2] == rhs[2] &&
	    lhs[3] == rhs[3] &&
	    lhs[4] == rhs[4];
	}
	friend bool operator!=(RfidTag& lhs, RfidTag& rhs) { return !(lhs == rhs); }
	unsigned char& operator[](int index) { return tagData[index]; }
	String16 toString() {
	    char str[16];
	    snprintf(str, 16*sizeof(char),
		     "%02x %02x %02x %02x %02x",
		     tagData[0], tagData[1], tagData[2], tagData[3], tagData[4]);
	    return str;
	}
	unsigned char checksum() {
	    unsigned char check = 0x00;
	    for (int i=0; i<5; i++)
		check ^= tagData[i];
	    return check;
	}
    };
}
