#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <string>
#include <vector>
#include <libserialport.h>

#ifndef ARDUINO_SERIAL_MAX_STR_LEN
#define ARDUINO_SERIAL_MAX_STR_LEN 128
#endif

#ifndef ARDUINO_SERIAL_WRITE_TIMEOUT
#define ARDUINO_SERIAL_WRITE_TIMEOUT 1000
#endif

#ifndef ARDUINO_SERIAL_READ_TIMEOUT
#define ARDUINO_SERIAL_READ_TIMEOUT 1000
#endif

#define METRO_MINI_VID 0x10c4
#define METRO_MINI_PID 0xea60

typedef struct sp_port* SerialPort;

class ArduinoSerial
{
 public:
    ArduinoSerial();
    ~ArduinoSerial();
    std::vector<SerialPort> findMatchingPorts(int vid, int pid);
    void openPort(SerialPort port, int baudRate);
    void closePort();
    void send(std::string key, std::string value);
    void update();
    void setDataCallback(void (*callback)(std::string, std::string));

 private:
    struct sp_port* serialPort;
    enum { WAIT_FOR_START,
           PARSE_KEY,
           PARSE_VALUE,
           N_PARSE_STATES
    } state;
    std::string key, value;
    void (*callback)(std::string, std::string);

    enum sp_return enumeratePorts(SerialPort** ports, int* n_ports);
    enum sp_return setArduinoConfig(SerialPort port, int baudrate);
    bool checkPort(SerialPort port, int vid, int pid);
    bool handshakePort(SerialPort port);
    void processChar(char c);
    void print_error(enum sp_return err);
};

#endif
