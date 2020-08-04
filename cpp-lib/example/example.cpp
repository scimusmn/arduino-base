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
        arduino.openPort(115200);
    }
    catch (std::runtime_error error) {
        std::cerr << "FATAL: " << error.what() << std::endl;
        return 1;
    }

    while(true) {
        arduino.update();
    }
}
