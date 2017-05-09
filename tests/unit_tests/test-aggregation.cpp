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
	auto grain1 = create_grain(g, g.entry(), "grn1");

	auto fork1 = create_fork(g, grain1, "f1");
	auto grain2 = create_grain(g, fork1, "grn2");

	auto fork2 = create_fork(g, grain2, "f2");
	auto grain3 = create_grain(g, fork2, "grn3");
	auto grain4 = create_grain(g, fork2, "grn4");
	auto join2 = create_join(g, {grain3,grain4}, "j2");

	auto grain6 = create_grain(g, fork1, "grn6");
	auto join3 = create_join(g, {join2, grain6}, "j3");

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
	auto grain1 = create_grain(g, g.entry(), "grn1");
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
	auto grain1 = create_grain(g, g.entry(), "grn1");

	auto fork1 = create_fork(g, grain1, "f1");
	auto grain2 = create_grain(g, fork1, "grn2");

	auto fork2 = create_fork(g, grain2, "f2");
	auto grain3 = create_grain(g, fork2, "grn3");
	auto grain4 = create_grain(g, fork2, "grn4");
	auto join2 = create_join(g, {grain3,grain4}, "j2");

	auto fork3 = create_fork(g, join2, "f3");
	auto grain5 = create_grain(g, fork3, "grn5");
	auto grain6 = create_grain(g, fork3, "grn6");
	auto grain7 = create_grain(g, fork3, "grn7");
	auto join3 = create_join(g, {grain5,grain6,grain7}, "j3");

	auto join4 = create_join(g, {join3}, "j4");

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
