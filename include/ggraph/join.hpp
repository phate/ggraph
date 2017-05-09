#ifndef GGRAPH_JOIN_HPP
#define GGRAPH_JOIN_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>
#include <string>

namespace ggraph {

class join final : public operation {
public:
	virtual
	~join();

	inline
	join(const std::string arg_name)
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
create_join(ggraph::graph & graph, const std::unordered_set<node*> & predecessors, std::string name)
{
	ggraph::join join(name);
	return graph.add_node(join, predecessors);
}

}

#endif
