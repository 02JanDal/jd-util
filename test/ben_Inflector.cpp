#define NONIUS_RUNNER
#include <nonius/nonius_single.h++>

#include "jd-util/Inflector.h"

using namespace JD::Util;

NONIUS_BENCHMARK("registerEN", [](nonius::chronometer meter) {
	meter.measure([]() { Inflector::registerEN(); Inflector("en").clear(); });
})

NONIUS_BENCHMARK("pluralize", [](nonius::chronometer meter) {
	Inflector::registerEN();
	meter.measure([]() { return Inflector().pluralize("word"); });
})

NONIUS_BENCHMARK("singularize", [](nonius::chronometer meter) {
	Inflector::registerEN();
	meter.measure([]() { return Inflector().pluralize("words"); });
})
