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
	grain()
	: operation({})
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_grain(const ggraph::node * n) noexcept
{
	return dynamic_cast<const ggraph::grain*>(&n->operation()) != nullptr;
}

static inline node *
create_grain(ggraph::graph & graph, node * parent)
{
	ggraph::grain grain;
	return graph.add_node(grain, {parent});
}

}

#endif
