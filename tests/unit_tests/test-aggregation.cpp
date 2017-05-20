#include "test-registry.hpp"

#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/view.hpp>

static inline void
check_grain_node(const ggraph::agg::node * n)
{
	assert(is_grain_type(n->type()));
	assert(n->nchildren() == 0);
}

static inline void
check_forkjoin_node(const ggraph::agg::node * n, size_t nchildren)
{
	assert(is_forkjoin_type(n->type()));
	assert(n->nchildren() == nchildren);
}

static inline void
check_linear_node(const ggraph::agg::node * n, size_t nchildren)
{
	assert(is_linear_type(n->type()));
	assert(n->nchildren() == nchildren);
}

static int
test1()
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

	assert(ggraph::is_valid(g));

	auto root = agg::aggregate(g);

	agg::view_str(*root, stdout);
	agg::view_graphml(*root, stdout);

	check_linear_node(root.get(), 2);

	auto child1 = root->child(0);
	check_grain_node(child1);

	auto child2 = root->child(1);
	check_forkjoin_node(child2, 2);
	check_grain_node(child2->child(0));
	check_grain_node(child2->child(1));

	return 0;
}

static int
test2()
{
	using namespace ggraph;

	/* FIXME: this graph is not valid */
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

	auto root = agg::aggregate(g);

	agg::view_str(*root, stdout);
	agg::view_graphml(*root, stdout);

	check_linear_node(root.get(), 2);

	auto child1 = root->child(0);
	check_forkjoin_node(child1, 2);

	auto grandchild1 = child1->child(0);
	check_forkjoin_node(grandchild1, 2);
	check_grain_node(grandchild1->child(0));
	check_grain_node(grandchild1->child(1));

	auto grandchild2 = child1->child(1);
	check_forkjoin_node(grandchild2, 2);
	check_grain_node(grandchild2->child(0));
	check_grain_node(grandchild2->child(1));

	auto child2 = root->child(1);
	check_grain_node(child2);

	return 0;
}

static int
test(const std::vector<std::string> &)
{
	test1();
	test2();

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/test-aggregation", {}, test);
