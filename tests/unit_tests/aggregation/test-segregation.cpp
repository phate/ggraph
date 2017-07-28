#include "test-registry.hpp"

#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/attribute.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/view.hpp>

static inline std::unordered_set<std::unique_ptr<ggraph::attribute>>
create_attributes(bool problematic)
{
	using namespace ggraph;

	std::unordered_set<std::unique_ptr<attribute>> attributes;
	attributes.insert(std::move(create_dblattribute("problematic", problematic ? 1.0 : 0.0)));
	return attributes;
}

static int
test(const std::vector<std::string> &)
{
	using namespace ggraph;

	graph g;
	auto g1 = create_grain(g, {}, g.entry());

	auto fork = create_fork(g, {}, g1);
	auto g2 = create_grain(g, create_attributes(true), fork);
	auto g3 = create_grain(g, create_attributes(false), fork);
	auto g4 = create_grain(g, create_attributes(false), fork);
	auto g5 = create_grain(g, create_attributes(true), fork);
	auto join = create_join(g, {}, {g2, g3, g4, g5});

	join->add_successor(g.exit());

//	view(g, stdout);
	assert(is_valid(g));

	auto root = agg::aggregate(g);
	propagate(*root);
	agg::view_str(*root, stdout);

	segregate(*root);
	agg::view_str(*root, stdout);

	assert(root->nchildren() == 2);
	assert(is_forkjoin(root->last_child()->operation()));

	auto fjn = root->last_child();
	assert(fjn->nchildren() == 3);

	auto fjn2 = fjn->last_child();
	assert(is_forkjoin(fjn2->operation()) && fjn2->nchildren() == 2);

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/aggregation/test-segregation", {}, test);
