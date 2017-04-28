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
	auto grain2 = create_grain(g, grain1);

	auto fork = create_fork(g, grain2);
	auto grain3 = create_grain(g, fork);
	auto grain4 = create_grain(g, fork);
	auto join = create_join(g, {grain3, grain4});

	auto grain5 = create_grain(g, join);
	grain5->add_successor(g.exit());

	assert(g.nnodes() == 9);

	assert(g.entry()->npredecessors() == 0);
	assert(g.entry()->nsuccessors() == 1);
	assert(g.entry()->successor(0) == grain1);

	assert(grain1->npredecessors() == 1);
	assert(grain1->predecessor(0) == g.entry());
	assert(grain1->nsuccessors() == 1);
	assert(grain1->successor(0) == grain2);

	assert(fork->nsuccessors() == 2);
	assert(join->npredecessors() == 2);

	assert(g.exit()->npredecessors() == 1);
	assert(g.exit()->nsuccessors() == 0);

	view(g, stdout);

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-ggraph", test);
