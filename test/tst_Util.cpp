#include "jd-util/Util.h"

#include <QRegularExpression>
#include <vector>

#include <catch.hpp>

using namespace JD::Util;

TEST_CASE("Range works") {
	std::vector<int> values;
	for (const int i : range(5)) {
		values.push_back(i);
	}
	REQUIRE(values == std::vector<int>({0, 1, 2, 3, 4}));
}

TEST_CASE("regexReplace") {
	REQUIRE(regexReplace("abcdef", QRegularExpression("[ace]"), [](const QString &str) { return str.toUpper(); }) == "AbCdEf");
	REQUIRE(regexReplace("abcdef", QRegularExpression("[ace]"), [](const QString &str) { return str + str; }) == "aabccdeef");
	REQUIRE(regexReplace("abcdef", QRegularExpression("[ace]"), [](const QString &str) { return ""; }) == "bdf");
}
