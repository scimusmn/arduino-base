#include <iostream>
#include "../ArduinoSerial.h"

void printMessage(std::string key, std::string value)
{
    std::cout << "{'" << key << "' : '" << value << "' }" << std::endl;
}

int main() {
    ArduinoSerial arduino;
    arduino.setDataCallback(printMessage);

    try {
	auto portList = arduino.findMatchingPorts(METRO_MINI_VID, METRO_MINI_PID);
	if (portList.size() > 0)
	    arduino.openPort(portList[0], 115200);
    }
    catch (std::runtime_error error) {
        std::cerr << "FATAL: " << error.what() << std::endl;
        return 1;
    }

    while(true) {
        arduino.update();
    }
}
