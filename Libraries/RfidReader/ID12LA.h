#ifndef SMM_RFID_CARD_H
#define SMM_RFID_CARD_H

#ifndef ID12LA_TX
#define ID12LA_TX 3
#endif

#include <SoftwareSerial.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct Card
{
    unsigned char id[5];
    unsigned char checksum;

    bool check()
    {
	int sum = 0;
	for (int i=0; i<5; i++)
	    sum ^= id[i];

	return sum == checksum;
    }

    String getId()
    {
        char buf[15];
        snprintf(buf, 15*sizeof(char),
                 "%02x %02x %02x %02x %02x",
                 id[0], id[1], id[2], id[3], id[4]);
        return String(buf);
    }

    inline bool operator==(const Card& a)
    {
	if (a.id[0] != id[0])
	    return false;
	if (a.id[1] != id[1])
	    return false;
	if (a.id[2] != id[2])
	    return false;
	if (a.id[3] != id[3])
	    return false;
	if (a.id[4] != id[4])
	    return false;
	return true;
    }
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ID12LA
{
public:
    void setup(int rxPin, void (*cb)(Card&, void*), void* data)
    {
	serial = new SoftwareSerial(rxPin, ID12LA_TX);
	serial->begin(9600);

	bufferIndex = 0;
	status = WAITING;

	callback = cb;
	callbackData = data;
    }

    void update()
    {
	while (serial->available()) {
	    char c = serial->read();

	    if (status == READING)
		addChar(c);
	    else {
		if (c == 0x02)
		    status = READING;
	    }
	}
    }

private:
    SoftwareSerial *serial;
    
    enum { READING, WAITING } status;
    char buffer[15];
    int bufferIndex;
    Card card;
        
    void (*callback)(Card&, void*);
    void* callbackData;

    void addChar(char c)
    {
	buffer[bufferIndex] = c;
	bufferIndex += 1;
	if (bufferIndex >= 15) {
	    bufferIndex = 0;
	    status = WAITING;
	    processBuffer();
	}
    }

    unsigned char parse(int startIndex) {
	char str[3];
	str[0] = buffer[startIndex];
	str[1] = buffer[startIndex + 1];
	str[2] = 0;

	return strtoul(str, NULL, 16);
    }

    void processBuffer()
    {
	// ensure buffer matches expected characters
	if (buffer[12] != 0x0d ||
	    buffer[13] != 0x0a ||
	    buffer[14] != 0x03) {
	    return; // bad read, go no further
	}

	// parse buffer from ascii hex into numbers
	for (int i=0; i<5; i++)
	    card.id[i] = parse(2*i);

	card.checksum = parse(10);

	if (!card.check()) {
	    return; // bad checksum
	}

	callback(card, callbackData);
    }
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif
