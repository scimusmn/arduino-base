#pragma once

typedef unsigned char byte;


class PeripheralDevice {
public:
    virtual void onReceive(int num_bytes) = 0;
    virtual void onRequest() = 0;
};


class _Wire {
public:
    void begin() {}
    
    byte requestFrom(byte address, size_t quantity, bool stop=true) { return 0; }
    
    void beginTransmission(byte address) {}
    byte endTransmission(bool stop=true) { return 0; }

    // not spec-compliant but not needed now
    byte write(byte b) { return 0; }
    
    size_t available() { return 0; }
    byte read() { return 0; }
    
    void SetClock(unsigned long frequency) {}

    void reset() {}
    void attach(PeripheralDevice& device, byte address) {}
};

extern _Wire Wire;
