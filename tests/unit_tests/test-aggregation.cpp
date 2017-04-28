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
test1()
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

	view(g, stdout);

	aggregate(g);

	view(g, stdout);

	is_aggregated(g);

	return 0;
}

static int
test2()
{
	using namespace ggraph;

	graph g;
	auto fork1 = create_fork(g, g.entry());

	auto fork2 = create_fork(g, fork1);
	auto grain1 = create_grain(g, fork2);
	auto grain2 = create_grain(g, fork2);
	auto join2 = create_join(g, {grain1, grain2});

	auto fork3 = create_fork(g, fork1);
	auto grain3 = create_grain(g, fork3);
	auto grain4 = create_grain(g, fork3);
	auto join3 = create_join(g, {grain3, grain4});

	auto join1 = create_join(g, {join2, join3});
	auto grain5 = create_grain(g, join1);
	grain5->add_successor(g.exit());

	view(g, stdout);

	aggregate(g);

	view(g, stdout);

	is_aggregated(g);

	return 0;
}

static int
test()
{
	test1();
	test2();

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-aggregation", test);
