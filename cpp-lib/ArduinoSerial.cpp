#include <stdexcept>
#include <iostream>
#include <cstring>
#include "ArduinoSerial.h"

ArduinoSerial::ArduinoSerial()
{
    state = WAIT_FOR_START;
    key = "";
    value = "";
    callback = NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ArduinoSerial::~ArduinoSerial()
{
    closePort();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::vector<SerialPort> ArduinoSerial::findMatchingPorts(int vid, int pid)
{
    enum sp_return error;
    struct sp_port** portList;
    int numPorts;
    std::vector<struct sp_port*> matchingPorts;

    error = enumeratePorts(&portList, &numPorts);
    if (error != SP_OK)
        throw std::runtime_error("failed to enumerate ports");

    for (int i=0; i<numPorts; i++) {
        if (checkPort(portList[i], vid, pid))
            matchingPorts.push_back(portList[i]);
    }
    return matchingPorts;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::openPort(SerialPort port, int baudRate)
{
    enum sp_return error;
    std::string portName = sp_get_port_name(port);

    error = sp_open(port, SP_MODE_READ_WRITE);
    if (error != SP_OK) {
        std::string errorString = "error opening port ";
        errorString += portName;
        throw std::runtime_error(errorString);
    }

    // port opened correctly
    error = setArduinoConfig(port, baudRate);
    if (error != SP_OK) {
        std::string errorString = "error configuring port ";
        errorString += portName;
        sp_close(port);
        throw std::runtime_error(errorString);
    }

    if (handshakePort(port))
        serialPort = port;
    else {
        std::string errorString = "handshake failed on port ";
        errorString += portName;
        sp_close(port);
        throw std::runtime_error(portName);
    }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ArduinoSerial::closePort()
{
    if (serialPort != NULL)
        sp_close(serialPort);
    serialPort = NULL;
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

enum sp_return ArduinoSerial::enumeratePorts(SerialPort** ports, int* n_ports)
{
    int n = 0;

    enum sp_return err = sp_list_ports(ports);
    for (; (*ports)[n]; n++);
    if (err != SP_OK) {
        printf("ERROR: could not get list of ports\n");
    }
    printf("\n");
    (*n_ports) = n;
    return err;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum sp_return ArduinoSerial::setArduinoConfig(SerialPort port, int baudrate)
{
    enum sp_return err;
    err = sp_set_baudrate(port, baudrate);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_bits(port, 8);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_parity(port, SP_PARITY_NONE);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_stopbits(port, 1);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_rts(port, SP_RTS_ON);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_cts(port, SP_CTS_IGNORE);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_dtr(port, SP_DTR_ON);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_dsr(port, SP_DSR_IGNORE);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    err = sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
    if ( err != SP_OK ) {
        printf("error encountered in setting baudrate\n");
        print_error(err);
        return err;
    }

    return SP_OK;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ArduinoSerial::checkPort(struct sp_port* port,
                              int vid,
                              int pid)
{
    enum sp_transport transportType = sp_get_port_transport(port);
    if (transportType != SP_TRANSPORT_USB)
        return false;

    int portvid, portpid;
    sp_get_port_usb_vid_pid(port, &portvid, &portpid);
    if ( ((vid != 0) && (portvid != vid)) ||
         ((pid != 0) && (portpid != pid)) )
        return false;

    return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ArduinoSerial::handshakePort(SerialPort port)
{
    struct sp_event_set* event_set;
    sp_new_event_set(&event_set);
    sp_add_port_events(event_set, port, SP_EVENT_RX_READY);
    sp_wait(event_set, 5000);
    sp_free_event_set(event_set);

    char buf[18];

    sp_blocking_write(port, "{", sizeof(char), ARDUINO_SERIAL_WRITE_TIMEOUT);
    sp_blocking_read(port, &buf, 18, 10000);
    if (strncmp(buf, "{arduino-ready:1}", 8) == 0) {
        std::cout << "found arduino on port " << sp_get_port_name(port) << std::endl;
        serialPort = port;
        return true;
    }
    return false;
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

void ArduinoSerial::print_error(enum sp_return err)
{
    switch (err) {
    case SP_ERR_ARG:
        printf("ERROR: invalid arguments!\n");
        break;
    case SP_ERR_FAIL:
        printf("ERROR: system error occured!\n");
        break;
    case SP_ERR_MEM:
        printf("ERROR: memory allocation error\n");
        break;
    case SP_ERR_SUPP:
        printf("ERROR: the requested operation is not supported by this device\n");
        break;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
