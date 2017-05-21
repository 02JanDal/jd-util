#include "jd-util/Inflector.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace JD::Util;

namespace Catch {
template <> std::string toString<QString>(const QString &str) { return str.toStdString(); }
}

TEST_CASE("Inflector") {
	Inflector infl("en");
	infl.registerEN();

	SECTION("camelize") {
		REQUIRE(infl.camelize("html") == "HTML");
		REQUIRE(infl.camelize("my_html") == "MyHTML");
		REQUIRE(infl.camelize("my_html", false) == "myHTML");
		REQUIRE(infl.camelize("my_http_delimited") == "MyHTTPDelimited");
		REQUIRE(infl.camelize("phds") == "Phds");

		REQUIRE(infl.camelize("term_util") == "TermUtil");
		REQUIRE(infl.camelize("term_util", false) == "termUtil");
		REQUIRE(infl.camelize("http_request") == "HTTPRequest");
		REQUIRE(infl.camelize("jd/util/inflector_data") == "Jd::Util::InflectorData");
	}

	SECTION("underscore") {
		REQUIRE(infl.underscore("MyHTML") == "my_html");
		REQUIRE(infl.underscore("PhdS") == "phd_s");
	}

	const std::vector<std::vector<QString>> data = {{"post", "posts"}, {"octopus", "octopi"}, {"sheep", "sheep"}, {"CamelOctopus", "CamelOctopi"}};
	SECTION("pluralize") {
		for (const auto elem : data) {
			REQUIRE(infl.pluralize(elem[0]) == elem[1]);
			REQUIRE(infl.pluralize(elem[1]) == elem[1]);
		}
		REQUIRE(infl.pluralize("words") == "words");
	}

	SECTION("singularize") {
		for (const auto elem : data) {
			REQUIRE(infl.singularize(elem[0]) == elem[0]);
			REQUIRE(infl.singularize(elem[1]) == elem[0]);
		}
		REQUIRE(infl.singularize("word") == "word");
	}

	SECTION("tableize") {
		REQUIRE(infl.tableize("RawScaledScorer") == "raw_scaled_scorers");
		REQUIRE(infl.tableize("ham_and_eggs") == "ham_and_eggs");
		REQUIRE(infl.tableize("fancyCategory") == "fancy_categories");
	}

	SECTION("classify") {
		REQUIRE(infl.classify("ham_and_eggs") == "HamAndEgg");
		REQUIRE(infl.classify("posts") == "Post");
	}

	SECTION("ordinal") {
		REQUIRE(infl.ordinal(1) == "st");
		REQUIRE(infl.ordinal(2) == "nd");
		REQUIRE(infl.ordinal(1002) == "nd");
		REQUIRE(infl.ordinal(1003) == "rd");
		REQUIRE(infl.ordinal(-11) == "th");
		REQUIRE(infl.ordinal(-1021) == "st");
	}
}
