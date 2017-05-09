#include "test-registry.hpp"

#include <ggraph/aggregation.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/view.hpp>

static inline void
is_aggregated(ggraph::graph & g)
{
	assert(g.entry()->npredecessors() == 0);
	assert(g.entry()->nsuccessors() == 1);

	auto s = g.entry()->successor(0);
	assert(s->npredecessors() == 1);
	assert(s->nsuccessors() == 1);

	s = s->successor(0);
	assert(s == g.exit());
	assert(g.exit()->nsuccessors() == 0);
}

static int
test3()
{
	using namespace ggraph;

	graph g;
	auto grain1 = create_grain(g, g.entry());

	auto fork1 = create_fork(g, grain1);
	auto grain2 = create_grain(g, fork1);

	auto fork2 = create_fork(g, grain2);
	auto grain3 = create_grain(g, fork2);
	auto grain4 = create_grain(g, fork2);
	auto join2 = create_join(g, {grain3,grain4});

	auto grain6 = create_grain(g, fork1);
	auto join3 = create_join(g, {join2, grain6});

	join3->add_successor(g.exit());

	view(g, stdout);

	aggregate(g);

	view(g, stdout);

	is_aggregated(g);

	return 0;
}

static int
test4()
{
	using namespace ggraph;

	graph g;
	auto grain1 = create_grain(g, g.entry());
	grain1->add_successor(g.exit());

	view(g, stdout);

	aggregate(g);

	view(g, stdout);

	is_aggregated(g);

	return 0;
}

static int
test5()
{
	using namespace ggraph;

	graph g;
	auto grain1 = create_grain(g, g.entry());

	auto fork1 = create_fork(g, grain1);
	auto grain2 = create_grain(g, fork1);

	auto fork2 = create_fork(g, grain2);
	auto grain3 = create_grain(g, fork2);
	auto grain4 = create_grain(g, fork2);
	auto join2 = create_join(g, {grain3,grain4});

	auto fork3 = create_fork(g, join2);
	auto grain5 = create_grain(g, fork3);
	auto grain6 = create_grain(g, fork3);
	auto grain7 = create_grain(g, fork3);
	auto join3 = create_join(g, {grain5,grain6,grain7});

	auto join4 = create_join(g, {join3});

	join4->add_successor(g.exit());

	view(g, stdout);

	aggregate(g);

	view(g, stdout);

	is_aggregated(g);

	return 0;
}

static int
test()
{
	test3();
	test4();
	test5();

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-aggregation", test);
