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
	fork()
	: operation({})
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_fork(const ggraph::node * n) noexcept
{
	return dynamic_cast<const ggraph::fork*>(&n->operation()) != nullptr;
}

static inline node *
create_fork(ggraph::graph & graph, node * parent)
{
	ggraph::fork fork;
	return graph.add_node(fork, {parent});
}

}

#endif
