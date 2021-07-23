#pragma once

#include <map>
#include <queue>

typedef unsigned char byte;
typedef void (*rxHandler)();
typedef void (*txHandler)(int);


class _Wire {
public:
    void begin() {}
    void begin(byte address) {}
    
    byte requestFrom(byte address, size_t quantity, bool stop=true) { return 0; }
    
    void beginTransmission(byte address) {}
    byte endTransmission(bool stop=true) { return 0; }

    // not spec-compliant but not needed now
    byte write(byte b) { return 0; }
    
    size_t available() { return 0; }
    byte read() { return 0; }
    
    void SetClock(unsigned long frequency) {}

    void onReceive(txHandler handler);
    void onRequest(rxHandler handler);
};


struct _WireBus {
    std::map<byte, _Wire&> bus;
    std::queue<byte> copi; // controller out, peripheral in
    std::queue<byte> cipo; // controller in, peripheral out
};

extern _WireBus WireBus;
