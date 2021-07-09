#pragma once

#include <string>
#include <queue>

typedef uint8_t byte;

struct _Serial {
    void begin(unsigned int baudrate) {}

    unsigned int available() { return 0; }

    byte read() { return 0; }

    void println(std::string line) {}

    void send(std::string data) {}

    std::string outbuffer;
    std::queue<byte> inbuffer;
};

_Serial Serial;

