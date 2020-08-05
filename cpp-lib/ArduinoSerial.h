/** @file */

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

/** A convenience typedef.
 *
 * Users shouldn't have to deal with the intricacies of how
 * libserialport operates, so this typedef allows you to
 * more easily ignore those details.
 */
typedef struct sp_port* SerialPort;

/** A class for managing Arduino serial communication.
 */
class ArduinoSerial
{
 public:

    /** (constructor) */
    ArduinoSerial();

    /** (destructor)
     *
     * This calls closePort(), so you don't need to call it explicitly
     * when your object goes out of scope.
     */
    ~ArduinoSerial();

    /** Get all matching serial ports.
     *
     * This function returns a vector of SerialPorts that use USB
     * transport and match the VID and PID provided.
     *
     * @param vid The VID to match, or zero to ignore VID.
     * @param pid The PID to match, or zero to ignore PID.
     *
     * @return A vector of SerialPorts matching the supplied information.
     *
     * @throws std::runtime_error If port enumeration fails, this throws and error.
     */
    std::vector<SerialPort> findMatchingPorts(int vid, int pid);

    /** Open a serial port for communication.
     *
     * @param port The port to open (usually identified by findMatchingPorts())
     * @param baudRate The baudrate to use for communication.
     *
     * @throws std::runtime_error If opening or configuring the port fails, or if
     * the Arduino fails to handshake correctly, throw an error.
     */
    void openPort(SerialPort port, int baudRate);

    /** Close the current serial port. */
    void closePort();

    /** Send a message to the Arduino.
     *
     * @param key The message key.
     * @param value The message value.
     *
     * @throws std::runtime_error If no port has been opened, throws an error.
     */
    void send(std::string key, std::string value);

    /** Check for and process incoming data.
     *
     * This function should be called as often as possible.
     */
    void update();

    /** Set the on data callback.
     *
     * When a new message is available, the callback function provided to
     * this function will be called.
     *
     * @param callback The data callback function to use.
     */
    void setDataCallback(void (*callback)(std::string, std::string));

 private:
    SerialPort serialPort;
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
