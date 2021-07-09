#pragma once

#include <string>
#include <queue>

typedef uint8_t byte;

struct _Serial {
    void begin(unsigned int baudrate) {}

    unsigned int available() { return inbuffer.size(); }

    byte read() {
	byte b = inbuffer.front();
	inbuffer.pop();
	return b;
    }

    void println(std::string line) {
	outbuffer += line + "\n";
    }

    void send(std::string data) {
	for (int i=0; i<data.length(); i++) {
	    inbuffer.push(data[i]);
	}
    }

    void reset() {
	outbuffer = "";
	inbuffer = std::queue<byte>();
    }

    std::string outbuffer;
    std::queue<byte> inbuffer;
};

extern _Serial Serial;
