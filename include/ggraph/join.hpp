#ifndef GGRAPH_JOIN_HPP
#define GGRAPH_JOIN_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class join final : public operation {
public:
	virtual
	~join();

	inline
	join(std::unordered_set<std::unique_ptr<attribute>> attributes)
	: operation(std::move(attributes))
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_join(const ggraph::operation & op) noexcept
{
	return dynamic_cast<const ggraph::join*>(&op) != nullptr;
}

static inline node *
create_join(
	ggraph::graph & graph,
	std::unordered_set<std::unique_ptr<attribute>> attributes,
	const std::unordered_set<node*> & predecessors)
{
	ggraph::join join(std::move(attributes));
	return graph.add_node(join, predecessors);
}

static inline node *
create_join(ggraph::graph & graph, std::unordered_set<std::unique_ptr<attribute>> attributes)
{
	ggraph::join join(std::move(attributes));
	return graph.add_node(join, {});
}

}

#endif
