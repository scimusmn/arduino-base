#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <string>
#include "arduino_util.h"

#ifndef ARDUINO_SERIAL_MAX_STR_LEN
#define ARDUINO_SERIAL_MAX_STR_LEN 128
#endif

#ifndef ARDUINO_SERIAL_WRITE_TIMEOUT
#define ARDUINO_SERIAL_WRITE_TIMEOUT 1000
#endif

#ifndef ARDUINO_SERIAL_READ_TIMEOUT
#define ARDUINO_SERIAL_READ_TIMEOUT 1000
#endif

class ArduinoSerial
{
 public:
    ArduinoSerial();
    ~ArduinoSerial();
    void openPort(int baudRate);
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

    void processChar(char c);
};

#endif
