#include "test-registry.hpp"

#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/join.hpp>
#include <ggraph/view.hpp>

static int
test(const std::vector<std::string>&)
{
	using namespace ggraph;

	graph g;

	auto g1 = create_grain(g, {}, g.entry());

	auto f1 = create_fork(g, {}, g1);
	auto g2 = create_grain(g, {}, f1);
	auto g3 = create_grain(g, {}, f1);
	auto g4 = create_grain(g, {}, f1);
	auto j1 = create_join(g, {}, {g2, g3, g4});

	auto f2 = create_fork(g, {}, j1);
	auto g5 = create_grain(g, {}, f2);
	auto g6 = create_grain(g, {}, f2);
	auto g7 = create_grain(g, {}, f2);
	auto g8 = create_grain(g, {}, f2);
	auto j2 = create_join(g, {}, {g5, g6, g7, g8});

	j2->add_successor(g.exit());

	view(g, stdout);
	assert(is_valid(g));

	auto root = agg::aggregate(g);
	agg::view_str(*root, stdout);

	agg::normalize(*root);
	agg::view_str(*root, stdout);

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/aggregation/test-normalization", {}, test);
