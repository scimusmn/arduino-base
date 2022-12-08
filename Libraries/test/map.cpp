#include "doctest.h"
#include "../map.hpp"


TEST_CASE("simple map insertion/recall") {
	smm::map<char, int, 3> m;
	CHECK(m.size() == 0);
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
