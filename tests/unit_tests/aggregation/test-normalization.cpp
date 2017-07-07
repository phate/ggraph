#include "test-registry.hpp"

#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/view.hpp>

static inline void
test_linear_linear_reduction()
{
	using namespace ggraph::agg;

	auto forkjoin = create_forkjoin_node(ggraph::fork({}), ggraph::join({}));
	forkjoin->add_child(std::move(create_grain_node(ggraph::grain({}))));
	forkjoin->add_child(std::move(create_grain_node(ggraph::grain({}))));

	auto grain = create_grain_node(ggraph::grain({}));
	auto linear = create_linear_node(std::move(forkjoin), std::move(grain));

	grain = create_grain_node(ggraph::grain({}));
	auto root = create_linear_node(std::move(grain), std::move(linear));

	view_str(*root, stdout);
	normalize(*root);
	view_str(*root, stdout);

	assert(root->nchildren() == 3);
	assert(is_grain_type(root->first_child()->type()));
	assert(is_forkjoin_type(root->first_child()->next_sibling()->type()));
	assert(is_grain_type(root->last_child()->type()));
}

static int
test(const std::vector<std::string> &)
{
	test_linear_linear_reduction();

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/aggregation/test-normalization", {}, test);
