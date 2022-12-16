#include "doctest.h"
#include "mock.h"
#include "../smm.h"


int k;
void f1() {
	k = 66;
}
void f2(const char *str) {
	if (strcmp(str, "hello") == 0)
		k = -1;
	else
		k = 1;
}
void f3(int n) {
	k = n;
}
void f4(float f) {
	k = (int)(100*f);
}


TEST_CASE("void serial callback") {
	k = 0;
	smm::SerialCallback cb(f1);
	cb("");
	CHECK(k == 66);
}


TEST_CASE("string serial callback") {
	k = 0;
	smm::SerialCallback cb(f2);
	cb("hello");
	CHECK(k == -1);
	cb("goodbye");
	CHECK(k == 1);
}


TEST_CASE("int serial callback") {
	k = 0;
	smm::SerialCallback cb(f3);
	cb(" 16 ");
	CHECK(k == 16);
	cb("0xff");
	CHECK(k == 255);
}


TEST_CASE("float serial callback") {
	k = 0;
	smm::SerialCallback cb(f4);
	cb("3.1415");
	CHECK(k == 314);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

TEST_CASE("SerialController correctly registers callbacks") {
	k = 0;
	smm::SerialCallback cb(f2);
	CHECK(smm::SerialController::RegisterCallback("serial-callback", &cb) == true);
	
	smm::SerialController::ExecuteCallback("serial-callback", "hello");
	CHECK(k == -1);
}


SERIAL_CALLBACK("auto-callback", int n) {
	k = n;
}

SERIAL_CALLBACK("auto-callback-2", void) {}


TEST_CASE("auto-registered callback") {
	k = 0;
	INFO("num callbacks: ", smm::SerialController::num_callbacks());
	smm::SerialController::ExecuteCallback("auto-callback", "15");
	CHECK(k == 15);
}


TEST_CASE("SmmSerial correctly eats characters") {
	k = 0;
	
	// malformed packet
	const char *str1 = "{auto-callback}";
	for (char *c = const_cast<char*>(str1); *c != 0; c++) {
		SmmSerial.eatCharacter(*c);
	}
	CHECK(k == 0);

	// correct packet, with malformed garbage around it
	const char *str2 = "{{{}::}{ {auto-callback:16}::}";
	for (char *c = const_cast<char*>(str2); *c != 0; c++) {
		SmmSerial.eatCharacter(*c);
	}
	CHECK(k == 16);
}
