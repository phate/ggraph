#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/join.hpp>
#include <ggraph/operation.hpp>

#include <unordered_map>

namespace ggraph {
namespace agg {

/* group operation */

class group final : public operation {
public:
	virtual
	~group()
	{}

	inline constexpr
	group(const ggraph::node * entry, const ggraph::node * exit)
	: operation()
	, exit_(exit)
	, entry_(entry)
	{}

	inline const ggraph::node *
	entry() const noexcept
	{
		return entry_;
	}

	inline const ggraph::node *
	exit() const noexcept
	{
		return exit_;
	}

	virtual std::string
	debug_string() const override
	{
		return "group";
	}

	virtual std::unique_ptr<operation>
	copy() const override
	{
		return std::make_unique<group>(*this);
	}

private:
	const ggraph::node * exit_;
	const ggraph::node * entry_;
};

static inline ggraph::node *
create_group(ggraph::graph & graph, ggraph::node * entry, ggraph::node * exit)
{
	ggraph::agg::group group(entry, exit);
	return graph.add_node(group, {});
}

/* aggregation */

static inline bool
is_grain(const ggraph::node * n)
{
	return dynamic_cast<const grain*>(&n->operation()) != nullptr;
}

static inline bool
is_fork(const ggraph::node * n)
{
	return dynamic_cast<const fork*>(&n->operation()) != nullptr;
}

static inline bool
is_join(const ggraph::node * n)
{
	return dynamic_cast<const join*>(&n->operation()) != nullptr;
}

static inline bool
is_group(const ggraph::node * n)
{
	return dynamic_cast<const group*>(&n->operation()) != nullptr;
}

static inline bool
is_entry(const ggraph::node * n)
{
	return dynamic_cast<const entry*>(&n->operation()) != nullptr;
}

static inline bool
is_exit(const ggraph::node * n)
{
	return dynamic_cast<const exit*>(&n->operation()) != nullptr;
}

static inline bool
is_grain_or_group(const ggraph::node * n)
{
	return is_grain(n) || is_group(n);
}

static inline ggraph::node *
reduce_fork(
	ggraph::graph & graph,
	ggraph::node * fork,
	std::unordered_map<ggraph::node*, std::unique_ptr<ggraph::agg::node>> & map)
{
	/* sanity checks */
	GGRAPH_DEBUG_ASSERT(is_fork(fork));
	GGRAPH_DEBUG_ASSERT(fork->nsuccessors() != 0);
	GGRAPH_DEBUG_ASSERT(fork->successor(0)->nsuccessors() == 1);
	GGRAPH_DEBUG_ASSERT(map.find(fork) == map.end());

	auto join = fork->successor(0)->successor(0);
	GGRAPH_DEBUG_ASSERT(is_join(join));
	GGRAPH_DEBUG_ASSERT(join->nsuccessors() == 1);
	GGRAPH_DEBUG_ASSERT(map.find(join) == map.end());

	for (size_t n = 0; n < fork->nsuccessors(); n++) {
		GGRAPH_DEBUG_ASSERT(fork->successor(n)->nsuccessors() == 1);
		GGRAPH_DEBUG_ASSERT(map.find(fork->successor(n)) != map.end());
		GGRAPH_DEBUG_ASSERT(fork->successor(n)->successor(0) == join);
	}

	/* perform reduction */
	auto group = create_group(graph, fork, join);
	fork->divert_predecessors(group);
	group->add_successor(join->successor(0));
	join->remove_successors();

	auto f = static_cast<const ggraph::fork*>(&fork->operation());
	auto j = static_cast<const ggraph::join*>(&join->operation());
	auto forkjoin = std::move(std::make_unique<forkjoin_node>(*f, *j));
	for (size_t n = 0; n < fork->nsuccessors(); n++)
		forkjoin->add_sibling(std::move(map[fork->successor(n)]));
	map[group] = std::move(forkjoin);

	return group;
}

static inline ggraph::node *
reduce_grain_or_group(
	ggraph::graph & graph,
	ggraph::node * entry,
	std::unordered_map<ggraph::node*, std::unique_ptr<ggraph::agg::node>> & map)
{
	/* sanity checks */
	GGRAPH_DEBUG_ASSERT(is_grain_or_group(entry));
	GGRAPH_DEBUG_ASSERT(entry->nsuccessors() == 1);
	GGRAPH_DEBUG_ASSERT(map.find(entry) != map.end());

	auto exit = entry->successor(0);
	GGRAPH_DEBUG_ASSERT(is_grain_or_group(exit));
	GGRAPH_DEBUG_ASSERT(exit->nsuccessors() == 1);
	GGRAPH_DEBUG_ASSERT(map.find(exit) != map.end());

	/* perform reduction */
	auto group = create_group(graph, entry, exit);
	entry->divert_predecessors(group);
	group->add_successor(exit->successor(0));
	exit->remove_successors();

	map[group] = std::make_unique<linear_node>(std::move(map[entry]), std::move(map[exit]));

	return group;
}

static inline void
aggregate(
	ggraph::graph & graph,
	ggraph::node * node,
	std::unordered_map<ggraph::node*, std::unique_ptr<ggraph::agg::node>> & map)
{
	if (is_grain_or_group(node) && is_fork(node->successor(0))) {
		aggregate(graph, node->successor(0), map);
		GGRAPH_DEBUG_ASSERT(is_group(node->successor(0)));
		auto group = reduce_grain_or_group(graph, node, map);
		aggregate(graph, group, map);
		return;
	}

	if (is_grain_or_group(node) && is_grain_or_group(node->successor(0))) {
		auto group = reduce_grain_or_group(graph, node, map);
		aggregate(graph, group, map);
		return;
	}

	if (is_fork(node)) {
		for (size_t n = 0; n < node->nsuccessors(); n++)
			aggregate(graph, node->successor(n), map);
		auto group = reduce_fork(graph, node, map);
		aggregate(graph, group, map);
		return;
	}

	if (is_join(node))
		return;

	if (node->nsuccessors() != 0) {
		GGRAPH_DEBUG_ASSERT(node->nsuccessors() == 1);
		aggregate(graph, node->successor(0), map);
	}
}

std::unique_ptr<agg::node>
aggregate(ggraph::graph & graph)
{
	/* insert all aggregation tree leaves into the map */
	std::unordered_map<ggraph::node*, std::unique_ptr<ggraph::agg::node>> map;
	for (size_t n = 0; n < graph.nnodes(); n++) {
		auto node = graph.node(n);
		if (is_grain(node)) {
			auto grain = static_cast<const ggraph::grain*>(&node->operation());
			map[node] = std::make_unique<grain_node>(*grain);
		}
	}

	aggregate(graph, graph.entry(), map);
	GGRAPH_DEBUG_ASSERT(map.find(graph.entry()->successor(0)) != map.end());
	return std::move(map[graph.entry()->successor(0)]);
}

}}
