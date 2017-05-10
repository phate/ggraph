#include "test-registry.hpp"

#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/view.hpp>

static int
test()
{
	using namespace ggraph;

	graph g;
	auto grain1 = create_grain(g, g.entry());

	auto fork = create_fork(g, grain1);
	auto grain3 = create_grain(g, fork);
	auto grain4 = create_grain(g, fork);
	auto join = create_join(g, {grain3, grain4});

	join->add_successor(g.exit());

	view(g, stdout);

	assert(is_valid(g));

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-ggraph", test);
