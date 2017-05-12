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

	auto fork = create_fork(g, g.entry());
	auto grain = create_grain(g, fork);
	auto join = create_join(g, {grain});

	join->add_successor(g.exit());

	view(g, stdout);

	assert(is_valid(g));

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-is_valid", test);
