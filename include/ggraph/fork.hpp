#ifndef GGRAPH_FORK_HPP
#define GGRAPH_FORK_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class fork final : public operation {
public:
	virtual
	~fork();

	inline
	fork(std::unordered_set<std::unique_ptr<attribute>> attributes)
	: operation(std::move(attributes))
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_fork(const ggraph::operation & op) noexcept
{
	return dynamic_cast<const ggraph::fork*>(&op) != nullptr;
}

static inline node *
create_fork(
	ggraph::graph & graph,
	std::unordered_set<std::unique_ptr<attribute>> attributes,
	node * parent)
{
	ggraph::fork fork(std::move(attributes));
	return graph.add_node(fork, {parent});
}

static inline node *
create_fork(ggraph::graph & graph, std::unordered_set<std::unique_ptr<attribute>> attributes)
{
	ggraph::fork fork(std::move(attributes));
	return graph.add_node(fork, {});
}

}

#endif
