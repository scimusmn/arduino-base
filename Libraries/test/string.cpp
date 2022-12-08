#include "doctest.h"
#include "../string.hpp"


TEST_CASE("basic operator= and c_str()") {
	smm::string<32> s;
	s = "hello, world!";
	CHECK(strcmp(s.c_str(), "hello, world!") == 0);
}


TEST_CASE("nice constructor=") {
	smm::string<32> s = "hello, world!";
	CHECK(strcmp(s.c_str(), "hello, world!") == 0);
}


TEST_CASE("constructor overflow") {
	CHECK_THROWS_AS(smm::string<2> s("hi"), smm::out_of_memory);
}
