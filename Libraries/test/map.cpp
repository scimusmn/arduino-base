#include "doctest.h"
#include "mock.h"
#include "../smm.h"


TEST_CASE("simple map insertion/recall") {
	smm::map<char, int, 3> m;
	CHECK(m.size() == 0);
	CHECK(m.max_size() == 3);
	CHECK(m.empty());

	m['a'] = 0;
	m['b'] = 1;
	m['c'] = 2;

	CHECK(m['a'] == 0);
	CHECK(m['b'] == 1);
	CHECK(m['c'] == 2);
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
	CHECK(m[10] == "hello");
	CHECK(m[30] == "sweet");
	CHECK(m[40] == "world");
	CHECK(m.contains(20) == false);
	CHECK(m.size() == 3);

	CHECK(m.erase(20) == 0);
	CHECK(m.erase(40) == 1);
	CHECK(m[10] == "hello");
	CHECK(m[30] == "sweet");
	CHECK(m.contains(20) == false);
	CHECK(m.contains(40) == false);
	CHECK(m.size() == 2);

	CHECK(m.erase(10) == 1);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(40) == 0);
	CHECK(m[30] == "sweet");
	CHECK(m.contains(10) == false);
	CHECK(m.contains(20) == false);
	CHECK(m.contains(40) == false);
	CHECK(m.size() == 1);

	CHECK(m.erase(10) == 0);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(30) == 1);
	CHECK(m.erase(40) == 0);
	CHECK(m.contains(10) == false);
	CHECK(m.contains(20) == false);
	CHECK(m.contains(30) == false);
	CHECK(m.contains(40) == false);
	CHECK(m.size() == 0);
	CHECK(m.empty());

	CHECK(m.erase(10) == 0);
	CHECK(m.erase(20) == 0);
	CHECK(m.erase(30) == 0);
	CHECK(m.erase(40) == 0);
}


TEST_CASE("contains()") {
	smm::map<int, const char *, 4> m;
	CHECK(m.contains(10) == false);
	m[10] = "hello!";
	CHECK(m.contains(10) == true);
}
