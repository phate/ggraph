#ifndef GGRAPH_AGGREGATION_HPP
#define GGRAPH_AGGREGATION_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class node;

class group final : public operation {
public:
	virtual
	~group();

	inline constexpr
	group(const node * entry, const node * exit)
	: operation()
	, exit_(exit)
	, entry_(entry)
	{}

	inline const node *
	entry() const noexcept
	{
		return entry_;
	}

	inline const node *
	exit() const noexcept
	{
		return exit_;
	}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;

private:
	const node * exit_;
	const node * entry_;
};

static inline node *
create_group(ggraph::graph & graph, node * entry, node * exit)
{
	ggraph::group group(entry, exit);
	return graph.add_node(group, {});
}

node *
aggregate(ggraph::graph & graph);

}

#endif
