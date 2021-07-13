#pragma once

typedef unsigned char uint8_t;

template<size_t numBytes>
class _EEPROM {
public:
    _EEPROM() {}
    uint8_t read(unsigned int index) { return 0; }
    void write(unsigned int index, uint8_t value) {}
    void update(unsigned int index, uint8_t value) {}
    uint8_t& operator[](unsigned int index) { return memory[0]; }
    void clear() {}
private:
    uint8_t memory[numBytes];
};

extern _EEPROM<2048> EEPROM;
