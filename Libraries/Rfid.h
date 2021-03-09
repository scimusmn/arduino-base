#ifndef SMM_RFID_H
#define SMM_RFID_H

// define before including to change
#ifndef MAX_RFID_READERS
#define MAX_RFID_READERS 16
#endif

#ifndef MAX_TAG_CATEGORIES
#define MAX_TAG_CATEGORIES 16
#endif

#define CATEGORY_TEACH_DEBOUNCE_TIME 20

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

struct Rfid;
void rfidTeach(struct Rfid *r, unsigned int category);
typedef void (*onreadCallback)(byte readerAddress, byte id[5], void* data);

bool idsAreEqual(byte id1[5], byte id2[5]) {
    if (id1[0] != id2[0])
	return false;
    if (id1[1] != id2[1])
	return false;
    if (id1[2] != id2[2])
	return false;
    if (id1[3] != id2[3])
	return false;
    if (id1[4] != id2[4])
	return false;
    return true;
}


String idToString(byte id[5]) {
    char buf[15];
    snprintf(buf, 15*sizeof(char),
	     "%02x %02x %02x %02x %02x",
	     id[0], id[1], id[2], id[3], id[4]);
    return String(buf);
}


bool readCard(byte address, byte id[5]) {
    bool isNewCard = false;
    Wire.requestFrom((int) address, 5);
    for (int i=0; i<5; i++) {
	id[i] = Wire.read();
	if (id[i] != 0)
	    isNewCard = true;
    }
    return isNewCard;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * struct TagCategory
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct TagCategory {
    unsigned int category;
    onreadCallback onread;
    void* data;
    int teachPin;
    bool state, debouncing;
    unsigned long debounceFinish;
};


void updateCategoryTeachPin(struct TagCategory *cat, struct Rfid *r) {
    if (!cat->debouncing) {
	bool state = digitalRead(cat->teachPin);
	if (!state && state != cat->state) {
	    // button was just pressed
	    cat->debouncing = true;
	    cat->debounceFinish = millis() + CATEGORY_TEACH_DEBOUNCE_TIME;
	}
	cat->state = state;
    }
    else if (millis() >= cat->debounceFinish) {
	cat->debouncing = false;
	rfidTeach(r, cat->category);
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * struct Tag                   
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct Tag {
    byte id[5];
    unsigned int category;
};


void readTag(struct Tag *tag, unsigned int address) {
    byte* t = (byte*) tag;
    for (int i=0; i<sizeof(struct Tag); i++)
	t[i] = EEPROM.read(address + i);
}


void writeTag(struct Tag *tag, unsigned int address) {
    byte* t = (byte*) tag;
    for (int i=0; i<sizeof(struct Tag); i++)
	EEPROM.update(address + i, t[i]);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * struct Rfid                      
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct Rfid {
    unsigned int numReaders;
    byte readers[MAX_RFID_READERS];

    unsigned int numCategories;
    struct TagCategory *categories;

    unsigned int numTags;
    struct Tag *tags;
    
    bool teaching;
    unsigned int teachingLedPin;
    byte unknownId[5];
    onreadCallback onunknown;
    void* unknownData;
};


struct Rfid * rfidSetup(unsigned int numCategories,
			int teachingLedPin,
			bool startWire) {
    struct Rfid *r = malloc(sizeof(struct Rfid));
    
    r->numReaders = 0;

    r->numCategories = numCategories;
    r->categories = calloc(numCategories, sizeof(TagCategory));

    // load tags from EEPROM
    r->numTags = EEPROM.read(0);
    if (r->numTags == 255)
	r->numTags = 0;
    r->tags = malloc(r->numTags * sizeof(struct Tag));

    struct Tag *currentTag = r->tags;
    unsigned int address = 1;

    for (int i=0; i<r->numTags; i++) {
	readTag(currentTag, address);
	currentTag += 1;
	address += sizeof(struct Tag);
    }

    r->teaching = false;
    pinMode(teachingLedPin, OUTPUT);
    r->teachingLedPin = teachingLedPin;

    r->onunknown = NULL;
    r->unknownData = NULL;

    if (startWire) {
	Wire.begin();
    }

    return r;
}


// teaching

void rfidSaveTags(struct Rfid *r) {
    EEPROM.update(0, r->numTags);

    int address = 1;
    struct Tag* tag = r->tags;
    for (int i=0; i<r->numTags; i++) {
	writeTag(tag, address);
	tag += 1;
	address += sizeof(struct Tag);
    }
}


void rfidTeach(struct Rfid *r, unsigned int category) {
    unsigned int numTags = r->numTags + 1;
    struct Tag *tags = malloc(numTags * sizeof(struct Tag));
    
    memcpy(tags[numTags - 1].id, r->unknownId, 5);
    memcpy(tags, r->tags, r->numTags * sizeof(struct Tag));
    tags[numTags - 1].category = category;

    free(r->numTags);
    r->numTags = numTags;
    r->tags = tags;
    
    rfidSaveTags(r);

    r->teaching = false;
    digitalWrite(r->teachingLedPin, 0);
}


void rfidWipeTags(struct Rfid *r) {
    r->numTags = 0;
    free(r->numTags);
    r->numTags = NULL;
    rfidSaveTags(r);
}


// readers

void rfidAddReader(struct Rfid *r, byte address) {
    if (r->numReaders < MAX_RFID_READERS) {
	r->readers[r->numReaders] = address;
	r->numReaders += 1;
    }
}


// categories

void rfidAddCategory(struct Rfid *r,
		     unsigned int category,
		     int teachPin,
		     onreadCallback onread) {
    TagCategory *cat = r->categories + category;
    cat->category = category;
    cat->onread = onread;
    cat->data = NULL;
    cat->teachPin = teachPin;
    pinMode(teachPin, INPUT_PULLUP);
    cat->state = true;
    cat->debouncing = false;
}


void rfidAddCategoryData(struct Rfid *r, unsigned int category, void* data) {
    r->categories[category].data = data;
}


int rfidGetCategory(struct Rfid *r, byte id[5]) {
    for (int i=0; i<r->numTags; i++) {
	struct Tag *tag = r->tags + i;
	if (idsAreEqual(id, tag->id))
	    return tag->category;
    }
    return -1;
}


// update

void rfidUpdate(struct Rfid *r) {
    byte id[5];
    
    for (int i=0; i<r->numReaders; i++) {
	if (readCard(r->readers[i], id)) {
	    int category = rfidGetCategory(r, id);

	    if (category < 0) {
		r->teaching = true;
		digitalWrite(r->teachingLedPin, HIGH);
		memcpy(r->unknownId, id, 5);
		if (r->onunknown != NULL) {
		    r->onunknown(r->readers[i], id, r->unknownData);
		}
	    }
	    else {
		r->teaching = false;
		digitalWrite(r->teachingLedPin, LOW);
		onreadCallback onread = r->categories[category].onread;
		void* data = r->categories[category].data;
		onread(r->readers[i], id, data);
	    }
	}
    }

    if (r->teaching) {
	for (int i=0; i<r->numCategories; i++) {
	    updateCategoryTeachPin(&(r->categories[i]), r);
	}
    }
}


#endif
