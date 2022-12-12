#include "doctest.h"
#include "../string.hpp"


TEST_CASE("basic operator= and c_str()") {
	smm::string<32> s;
	s = "hello, world!";
	CHECK(strcmp(s.c_str(), "hello, world!") == 0);
	CHECK(s.size() == strlen("hello, world!")+1);
	CHECK(s.max_size() == 32);
}


TEST_CASE("nice constructor=") {
	smm::string<32> s = "hello, world";
	CHECK(strcmp(s.c_str(), "hello, world") == 0);
	s.push_back('!');
	CHECK(strcmp(s.c_str(), "hello, world!") == 0);
}


TEST_CASE("constructor overflow") {
	CHECK_THROWS_AS(smm::string<2> s("hi"), smm::out_of_memory);
}


TEST_CASE("push more characters") {
	smm::string<4> s;
	CHECK(strcmp(s.c_str(), "") == 0);
	s.push_back('a');
	CHECK(strcmp(s.c_str(), "a") == 0);
	s.push_back('b');
	CHECK(strcmp(s.c_str(), "ab") == 0);
	s.push_back('c');
	CHECK(strcmp(s.c_str(), "abc") == 0);
	CHECK_THROWS_AS(s.push_back('d'), smm::out_of_memory);
}
