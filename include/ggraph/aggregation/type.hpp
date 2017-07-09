#ifndef GGRAPH_AGGREGATION_TYPE_HPP
#define GGRAPH_AGGREGATION_TYPE_HPP

#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>

#include <memory>
#include <string>

namespace ggraph {
namespace agg {

class node;

/* forkjoin operation */


/* linear operation */

class linear final : public operation {
public:
	virtual
	~linear();

	inline
	linear()
	: operation({})
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_linear(const ggraph::operation & operation) noexcept
{
	return dynamic_cast<const linear*>(&operation) != nullptr;
}

}}

#endif
