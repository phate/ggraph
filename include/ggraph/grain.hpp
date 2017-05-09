#ifndef GGRAPH_GRAIN_HPP
#define GGRAPH_GRAIN_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>
#include <string>

namespace ggraph {

class grain final : public operation {
public:
	virtual
	~grain();

	inline
	grain(const std::string arg_name)
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
create_grain(ggraph::graph & graph, node * parent, std::string name)
{
	ggraph::grain grain(name);
	return graph.add_node(grain, {parent});
}

}

#endif
