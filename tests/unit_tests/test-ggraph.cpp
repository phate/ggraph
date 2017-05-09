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
	auto grain1 = create_grain(g, g.entry(), "grn1");

	auto fork = create_fork(g, grain1, "f");
	auto grain3 = create_grain(g, fork, "grn3");
	auto grain4 = create_grain(g, fork, "grn4");
	auto join = create_join(g, {grain3, grain4}, "j");

	join->add_successor(g.exit());

	assert(g.nnodes() == 7);

	assert(g.entry()->npredecessors() == 0);
	assert(g.entry()->nsuccessors() == 1);
	assert(g.entry()->successor(0) == grain1);

	assert(grain1->npredecessors() == 1);
	assert(grain1->predecessor(0) == g.entry());
	assert(grain1->nsuccessors() == 1);
	assert(grain1->successor(0) == fork);

	assert(fork->nsuccessors() == 2);
	assert(join->npredecessors() == 2);

	assert(g.exit()->npredecessors() == 1);
	assert(g.exit()->nsuccessors() == 0);

        // TODO: Test these global invariants
        // - Fork predecessor is either join or grain
        // - Fork successors are grains
        // - Join predessors are either grains or joins
        // - Join successor is either join or fork
        // - Grain predessor is a fork
        // - Grain successor is a join

	view(g, stdout);

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-ggraph", test);
