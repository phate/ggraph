#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>

namespace ggraph {

/* group operation */

group::~group()
{}

std::string
group::debug_string() const
{
	return "group";
}

std::unique_ptr<operation>
group::copy() const
{
	return std::make_unique<group>(*this);
}

/* aggregation */

static inline bool
is_grain(const node * n)
{
	return dynamic_cast<const grain*>(&n->operation()) != nullptr;
}

static inline bool
is_fork(const node * n)
{
	return dynamic_cast<const fork*>(&n->operation()) != nullptr;
}

static inline bool
is_join(const node * n)
{
	return dynamic_cast<const join*>(&n->operation()) != nullptr;
}

static inline bool
is_group(const node * n)
{
	return dynamic_cast<const group*>(&n->operation()) != nullptr;
}

static inline bool
is_entry(const node * n)
{
	return dynamic_cast<const entry*>(&n->operation()) != nullptr;
}

static inline bool
is_exit(const node * n)
{
	return dynamic_cast<const exit*>(&n->operation()) != nullptr;
}

static inline bool
is_grain_or_group(const node * n)
{
	return is_grain(n) || is_group(n);
}

static inline node *
reduce_fork(ggraph::graph & graph, node * fork)
{
	/* sanity checks */
	GGRAPH_DEBUG_ASSERT(is_fork(fork));
	GGRAPH_DEBUG_ASSERT(fork->nsuccessors() != 0);
	GGRAPH_DEBUG_ASSERT(fork->successor(0)->nsuccessors() == 1);

	auto join = fork->successor(0)->successor(0);
	GGRAPH_DEBUG_ASSERT(is_join(join));
	GGRAPH_DEBUG_ASSERT(join->nsuccessors() == 1);

	for (size_t n = 0; n < fork->nsuccessors(); n++) {
		GGRAPH_DEBUG_ASSERT(fork->successor(n)->nsuccessors() == 1);
		GGRAPH_DEBUG_ASSERT(fork->successor(n)->successor(0) == join);
	}

	/* perform reduction */
	auto group = create_group(graph, fork, join);
	fork->divert_predecessors(group);
	group->add_successor(join->successor(0));
	join->remove_successors();

	return group;
}

static inline node *
reduce_grain_or_group(ggraph::graph & graph, node * entry)
{
	/* sanity checks */
	GGRAPH_DEBUG_ASSERT(is_grain(entry) || is_group(entry));
	GGRAPH_DEBUG_ASSERT(entry->nsuccessors() == 1);

	auto exit = entry->successor(0);
	GGRAPH_DEBUG_ASSERT(is_grain(exit) || is_group(exit));
	GGRAPH_DEBUG_ASSERT(exit->nsuccessors() == 1);

	/* perform reduction */
	auto group = create_group(graph, entry, exit);
	entry->divert_predecessors(group);
	group->add_successor(exit->successor(0));
	exit->remove_successors();

	return group;
}

static inline void
aggregate(ggraph::graph & graph, ggraph::node * node)
{
	if (is_grain_or_group(node) && is_fork(node->successor(0))) {
		aggregate(graph, node->successor(0));
		GGRAPH_DEBUG_ASSERT(is_group(node->successor(0)));
		auto group = reduce_grain_or_group(graph, node);
		aggregate(graph, group);
		return;
	}

	if (is_grain_or_group(node) && is_grain_or_group(node->successor(0))) {
		auto group = reduce_grain_or_group(graph, node);
		aggregate(graph, group);
		return;
	}

	if (is_fork(node)) {
		for (size_t n = 0; n < node->nsuccessors(); n++)
			aggregate(graph, node->successor(n));
		auto group = reduce_fork(graph, node);
		aggregate(graph, group);
		return;
	}

	if (is_join(node))
		return;

	if (node->nsuccessors() != 0) {
		GGRAPH_DEBUG_ASSERT(node->nsuccessors() == 1);
		aggregate(graph, node->successor(0));
	}
}

node *
aggregate(ggraph::graph & graph)
{
	aggregate(graph, graph.entry());
	return graph.entry()->successor(0);
}

}
