#ifndef GGRAPH_FORK_HPP
#define GGRAPH_FORK_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>
#include <string>

namespace ggraph {

class fork final : public operation {
public:
	virtual
	~fork();

	inline
	fork(const std::string arg_name)
	: operation(), name(arg_name)
        {}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;

private:
        std::string name;
};

static inline node *
create_fork(ggraph::graph & graph, node * parent, std::string name)
{
	ggraph::fork fork(name);
	return graph.add_node(fork, {parent});
}

}

#endif
