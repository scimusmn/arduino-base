#include "SerialController.hpp"

SerialController serialController;
unsigned long baudrate = 115200;

void onParse(char* key, char* val) {}

void setup()
{
    serialController.setup(baudrate, &onParse);
}

void loop() {
    static int count = 0;
    serialController.sendMessage("count", count);
    count++;
    delay(1000);
}
