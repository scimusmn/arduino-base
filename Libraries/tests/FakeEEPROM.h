#pragma once

#include <string.h>

typedef unsigned char uint8_t;

template<size_t numBytes>
class _EEPROM {
public:
    _EEPROM() { clear(); }
    uint8_t read(unsigned int index) { return memory[index]; }
    void write(unsigned int index, uint8_t value) { memory[index] = value; }
    void update(unsigned int index, uint8_t value) { memory[index] = value; }
    uint8_t& operator[](unsigned int index) { return memory[index]; }
    void clear() { memset(memory, 255, numBytes); }
private:
    uint8_t memory[numBytes];
};

extern _EEPROM<2048> EEPROM;
