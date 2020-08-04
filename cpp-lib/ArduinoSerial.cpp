#include <stdexcept>
#include <iostream>
#include "ArduinoSerial.h"

ArduinoSerial::ArduinoSerial()
{
    state = WAIT_FOR_START;
    key = "";
    value = "";
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ArduinoSerial::~ArduinoSerial()
{
    closePort();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::openPort(int baudRate)
{
    enum sp_return error;
    struct sp_port** portList;
    int numPorts;

    error = enumerate_ports(&portList, &numPorts);
    if (error != SP_OK)
        throw std::runtime_error("failed to enumerate ports");

    if (numPorts == 0)
        throw std::runtime_error("no serial devices found");

    for (int i=0; i<numPorts; i++) {
        char buf = 0;
        error = sp_open(portList[i], SP_MODE_READ_WRITE);
        if (error != SP_OK) {
            std::cerr << "WARNING: error trying to open port " << sp_get_port_name(portList[i]) << std::endl;
            continue;
        }

        // port opened correctly
        error = set_arduino_config(portList[i], baudRate);
        if (error != SP_OK) {
            std::cerr << "WARNING: error trying to configure port " << sp_get_port_name(portList[i]) << std::endl;
            sp_close(portList[i]);
            continue;
        }

        // port configured correctly, check handshake
        std::cout << "scanning port " << sp_get_port_name(portList[i]) << std::endl;
        sp_blocking_write(portList[i], "{", sizeof(char), ARDUINO_SERIAL_WRITE_TIMEOUT);
        sp_blocking_read(portList[i], &buf, 1, 10000);
        if (buf == '{') {
            std::cout << "found arduino on port " << sp_get_port_name(portList[i]) << std::endl;
            serialPort = portList[i];
            return;
        }

        sp_close(portList[i]);
        std::cout << "closed port." << std::endl;
    }
    throw std::runtime_error("no arduino found!");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::closePort()
{
    if (serialPort != NULL)
        sp_close(serialPort);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::send(std::string key, std::string value)
{
    if (serialPort == NULL)
        throw std::runtime_error("no port opened!");

    std::string buffer = "{";
    
    if (key.length() >= ARDUINO_SERIAL_MAX_STR_LEN)
        buffer += key.substr(0, ARDUINO_SERIAL_MAX_STR_LEN);
    else
        buffer += key;

    buffer += ":";

    if (value.length() >= ARDUINO_SERIAL_MAX_STR_LEN)
        buffer += value.substr(0, ARDUINO_SERIAL_MAX_STR_LEN);
    else
        buffer += value;

    buffer += "}";

    sp_blocking_write(serialPort,
                      buffer.c_str(),
                      buffer.length() * sizeof(char),
                      ARDUINO_SERIAL_WRITE_TIMEOUT);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::update()
{
    int numBytes = sp_input_waiting(serialPort);
    if (numBytes > 0) {
        char* buffer = (char*) calloc(numBytes, sizeof(char));
        if (buffer == NULL)
            throw std::bad_alloc();

        sp_blocking_read(serialPort,
                         buffer,
                         numBytes * sizeof(char),
                         ARDUINO_SERIAL_READ_TIMEOUT);
        for (int i=0; i<numBytes; i++) {
            processChar(buffer[i]);
        }
        free(buffer);
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::setDataCallback(void (*callback)(std::string, std::string))
{
    this->callback = callback;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::processChar(char c)
{
    // check for premature message start
    if (c == '{' && state != WAIT_FOR_START) {
        std::cerr << "WARNING: ignoring unterminated message!" << std::endl;
        state = PARSE_KEY;
        key = "";
        value = "";
        return;
    }

    // check for premature message end
    if (c == '}' &&
        state != PARSE_VALUE) {
        std::cerr << "WARNING: ignoring prematurely terminated message!" << std::endl;
        state = WAIT_FOR_START;
        return;
    }

    // process char as normal
    switch(state) {
    case WAIT_FOR_START:
        if (c == '{') {
            state = PARSE_KEY;
            key = "";
            value = "";
        }
        break;

        // ~~~~~~~~~~~~~~~~

    case PARSE_KEY:
        if (c == ':')
            state = PARSE_VALUE;
        else if (key.length() < ARDUINO_SERIAL_MAX_STR_LEN)
            key += c;
        break;

        // ~~~~~~~~~~~~~~~~

    case PARSE_VALUE:
        if (c == '}') {
            if (callback != NULL)
                callback(key, value);
            state = WAIT_FOR_START;
        }
        else if (value.length() < ARDUINO_SERIAL_MAX_STR_LEN)
            value += c;
        break;

        // ~~~~~~~~~~~~~~~~

    default:
        // we should never get here! reset state
        state = WAIT_FOR_START;
        break;
    }
}
            
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
