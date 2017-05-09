#ifndef GGRAPH_AGGREGATION_HPP
#define GGRAPH_AGGREGATION_HPP

#include <ggraph/graph.hpp>
#include <ggraph/operation.hpp>
#include <string>

namespace ggraph {

class node;

class group final : public operation {
public:
	virtual
	~group();

	inline
	group(const node * entry, const node * exit, const std::string arg_name)
	: operation()
	, exit_(exit)
	, entry_(entry)
        , name(arg_name)
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
        std::string name;
};


node *
create_group(ggraph::graph & graph, node * entry, node * exit);

node *
aggregate(ggraph::graph & graph);

}

#endif
