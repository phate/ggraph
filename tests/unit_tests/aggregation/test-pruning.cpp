#include "test-registry.hpp"

#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/view.hpp>

static int
test(const std::vector<std::string> &)
{
	using namespace ggraph::agg;

	auto grain = create_grain_node(ggraph::grain());
	auto linear = std::make_unique<node>(std::make_unique<linear_type>());
	auto root = create_linear_node(std::move(linear), std::move(grain));

	view_str(*root, stdout);
	prune(*root);
	view_str(*root, stdout);

	assert(root->nchildren() == 1);
	assert(is_grain_type(root->first_child()->type()));

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/aggregation/test-pruning", {}, test);
