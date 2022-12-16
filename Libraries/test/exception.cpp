#include "doctest.h"
#include "mock.h"
#include "../smm.h"

TEST_CASE("base exception class has correct what()") {
	smm::exception e;
	CHECK(e.what() == "Unknown exception");
}


TEST_CASE("base exception works correctly when throwing") {
	try {
		throw smm::exception("exception string");
	}
	catch(smm::exception& e) {
		CHECK(e.what() == "exception string");
	}
}


TEST_CASE("out_of_memory is caught by runtime_error") {
	try {
		throw smm::out_of_memory("out of memory!");
	}
	catch(smm::runtime_error& e) {
		CHECK(e.what() == "out of memory!");
	}
}
