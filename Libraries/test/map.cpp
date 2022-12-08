#include "doctest.h"
#include "../map.hpp"


TEST_CASE("simple map insertion/recall") {
	smm::map<char, int, 3> m;
	CHECK(m.size() == 0);
	CHECK(m.max_size() == 3);
	CHECK(m.empty());

	CHECK_NOTHROW(m['a'] = 0);
	CHECK_NOTHROW(m['b'] = 1);
	CHECK_NOTHROW(m['c'] = 2);
	CHECK_THROWS_AS(m['d'], smm::out_of_memory);

	CHECK(m.at('a') == 0);
	CHECK(m.at('b') == 1);
	CHECK(m.at('c') == 2);

	CHECK_THROWS_AS(m.at('q'), smm::out_of_range);
}


TEST_CASE("erasing values") {
	smm::map<int, const char *, 4> m;
	CHECK(m.size() == 0);
	
	m[10] = "hello";
	m[20] = "my";
	m[30] = "sweet";
	m[40] = "world";

	CHECK(m.size() == 4);
	CHECK(m.erase(20) == 1);
	CHECK(m.at(10) == "hello");
	CHECK(m.at(30) == "sweet");
	CHECK(m.at(40) == "world");
	CHECK_THROWS_AS(m.at(20), smm::out_of_range);
	CHECK(m.size() == 3);

	CHECK(m.erase(20) == 0);
	CHECK(m.erase(40) == 1);
	CHECK(m.at(10) == "hello");
	CHECK(m.at(30) == "sweet");
	CHECK_THROWS_AS(m.at(20), smm::out_of_range);
	CHECK_THROWS_AS(m.at(40), smm::out_of_range);
	CHECK(m.size() == 2);

	CHECK(m.erase(10) == 1);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(40) == 0);
	CHECK(m.at(30) == "sweet");
	CHECK_THROWS_AS(m.at(10), smm::out_of_range);
	CHECK_THROWS_AS(m.at(20), smm::out_of_range);
	CHECK_THROWS_AS(m.at(40), smm::out_of_range);
	CHECK(m.size() == 1);

	CHECK(m.erase(10) == 0);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(30) == 1);
	CHECK(m.erase(40) == 0);
	CHECK_THROWS_AS(m.at(10), smm::out_of_range);
	CHECK_THROWS_AS(m.at(20), smm::out_of_range);
	CHECK_THROWS_AS(m.at(30), smm::out_of_range);
	CHECK_THROWS_AS(m.at(40), smm::out_of_range);
	CHECK(m.size() == 0);
	CHECK(m.empty());

	CHECK(m.erase(10) == 0);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(30) == 0);
	CHECK(m.erase(40) == 0);
	
}
