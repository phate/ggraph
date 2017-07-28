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
	auto g1 = create_grain(g, {create_attributes(false)}, g.entry());

	auto f1 = create_fork(g, {}, g1);
	auto g2 = create_grain(g, create_attributes(true), f1);
	auto g3 = create_grain(g, create_attributes(false), f1);
	auto g4 = create_grain(g, create_attributes(false), f1);
	auto g5 = create_grain(g, create_attributes(true), f1);
	auto j1 = create_join(g, {}, {g2, g3, g4, g5});

	auto f2 = create_fork(g, {}, j1);
	auto g6 = create_grain(g, create_attributes(false), f2);
	auto j2 = create_join(g, {}, {g6});

	auto f3 = create_fork(g, {}, j2);
	auto g7 = create_grain(g, create_attributes(false), f3);
	auto j3 = create_join(g, {}, {g7});

	j3->add_successor(g.exit());

//	view(g, stdout);
	assert(is_valid(g));

	auto root = agg::aggregate(g);
	normalize(*root);
	propagate(*root);
	agg::view_str(*root, stdout);

	segregate(*root);
	agg::view_str(*root, stdout);

	assert(root->nchildren() == 3);
	assert(is_grain(root->first_child()->operation()));

	auto fjn = root->first_child()->next_sibling();
	assert(is_forkjoin(fjn->operation()));
	assert(fjn->nchildren() == 3);
	auto fjn2 = fjn->last_child();
	assert(is_forkjoin(fjn2->operation()) && fjn2->nchildren() == 2);

	auto ln = root->last_child();
	assert(ln->nchildren() == 2);
	assert(is_forkjoin(ln->first_child()->operation()));
	assert(is_forkjoin(ln->last_child()->operation()));

	return 0;
}

GGRAPH_TEST_REGISTER("unit_tests/aggregation/test-segregation", {}, test);
