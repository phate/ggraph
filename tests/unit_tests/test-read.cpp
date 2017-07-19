#include "test-registry.hpp"

#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/read.hpp>
#include <ggraph/view.hpp>

static int
test(const std::vector<std::string> & files)
{
	using namespace ggraph;

	for (const auto & file : files) {
		auto f = fopen(file.c_str(), "r");
		assert(f != 0);

		auto g = read_graphml(f);
		assert(is_valid(*g));

		fclose(f);
	}

	return 0;
}

#define C ,

GGRAPH_TEST_REGISTER(
	"unit_tests/test-read",
	{"tests/graphs/tiny.graphml"},
	test);
