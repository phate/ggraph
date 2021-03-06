#ifndef GGRAPH_GRAIN_HPP
#define GGRAPH_GRAIN_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class grain final : public operation {
public:
	virtual
	~grain();

	inline
	grain(std::unordered_set<std::unique_ptr<attribute>> attributes)
	: operation(std::move(attributes))
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_grain(const ggraph::operation & op) noexcept
{
	return dynamic_cast<const ggraph::grain*>(&op) != nullptr;
}

static inline node *
create_grain(
	ggraph::graph & graph,
	std::unordered_set<std::unique_ptr<attribute>> attributes,
	node * parent)
{
	ggraph::grain grain(std::move(attributes));
	return graph.add_node(grain, {parent});
}

static inline node *
create_grain(ggraph::graph & graph, std::unordered_set<std::unique_ptr<attribute>> attributes)
{
	ggraph::grain grain(std::move(attributes));
	return graph.add_node(grain, {});
}

}

#endif
