#ifndef GGRAPH_JOIN_HPP
#define GGRAPH_JOIN_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class join final : public operation {
public:
	virtual
	~join();

	inline constexpr
	join()
	: operation()
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline node *
create_join(ggraph::graph & graph, const std::unordered_set<node*> & predecessors)
{
	ggraph::join join;
	return graph.add_node(join, predecessors);
}

}

#endif
