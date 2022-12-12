#include "doctest.h"
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
