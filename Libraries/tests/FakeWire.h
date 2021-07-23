#pragma once

#include <map>
#include <queue>

typedef unsigned char byte;
typedef void (*rxHandler)();
typedef void (*txHandler)(int);


class _Wire;

struct _WireBus {
    std::map<byte, _Wire*> bus;
    std::queue<byte> copi; // controller out, peripheral in
    std::queue<byte> cipo; // controller in, peripheral out
};

extern _WireBus WireBus;


class _Wire {
public:
    _Wire() { rx = nullptr; tx = nullptr; }
    ~_Wire() {
	WireBus.bus.erase(address);
    }
	
    void begin() { isController = true; }
    void begin(byte address) {
	isController = false;
	this->address = address;
	WireBus.bus[address] = this;
    }
    
    
    byte requestFrom(byte address, size_t quantity, bool stop=true) {
	int previouslyAvailable = available();
	try {
	    _Wire *peripheral = WireBus.bus.at(address);
	    rxHandler rx = peripheral->getRx();
	    if (rx != nullptr)
		rx();
	    return available() - previouslyAvailable;
	} catch(...) {
	    return 0;
	}
    }
    
    void beginTransmission(byte address) {
	if (!isController) return;
	this->address = address;
	txSize = 0;
    }
    
    byte endTransmission(bool stop=true) {
	if (!isController) return 0;
	try {
	    _Wire *peripheral = WireBus.bus.at(address);
	    txHandler tx = peripheral->getTx();
	    if (tx == nullptr)
		return 3; // "received NACK on transmit of data"
	    tx(txSize);
	    return 0; // "success"
	} catch(...) {
	    return 2; // "received NACK on transmit of address"
	}
    }

    
    // not spec-compliant but not needed now
    byte write(byte b) {
	if (isController) {
	    WireBus.copi.push(b);
	    txSize++;
	}
	else { // peripheral
	    WireBus.cipo.push(b);
	}
	return 1;
    }
    
    size_t available() {
	if (isController)
	    return WireBus.cipo.size();
	else // peripheral
	    return WireBus.copi.size();
    }
    
    byte read() {
	if (isController) {
	    byte b = WireBus.cipo.front();
	    WireBus.cipo.pop();
	    return b;
	}
	else { // peripheral
	    byte b = WireBus.copi.front();
	    WireBus.copi.pop();
	    return b;
	}
    }
    
    void SetClock(unsigned long frequency) {}

    void onReceive(txHandler handler) { tx = handler; }
    void onRequest(rxHandler handler) { rx = handler; }

    rxHandler getRx() { return rx; }
    txHandler getTx() { return tx; }
private:
    bool isController;
    int txSize;
    rxHandler rx;
    txHandler tx;
    byte address;
};


