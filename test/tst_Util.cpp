#include "jd-util/Util.h"

#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace JD::Util;

TEST_CASE("Range works") {
	std::vector<int> values;
	for (const int i : range(5)) {
		values.push_back(i);
	}
	REQUIRE(values == std::vector<int>({0, 1, 2, 3, 4}));
}
