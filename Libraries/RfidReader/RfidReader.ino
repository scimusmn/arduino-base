#include <Wire.h>
#include "ID12LA.h"

#define PERIPHERAL_ADDRESS 0x70
#define LED_PIN 10
#define READER_PIN 0

ID12LA reader;
Card card;

void onread(Card& c, void* data)
{
    card = c;
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW);
}

void setup() {
    reader.setup(READER_PIN, onread, NULL);
    memset(card.id, 0x00, 5);

    pinMode(LED_PIN, OUTPUT);
    
    Wire.begin(PERIPHERAL_ADDRESS);
    Wire.onRequest(requestEvent);
}

void loop()
{
    reader.update();
}

void requestEvent()
{
    Wire.write(card.id, sizeof(card.id));
    memset(card.id, 0x00, 5);
}
	    
