#ifndef GGRAPH_AGGREGATION_LINEAR_HPP
#define GGRAPH_AGGREGATION_LINEAR_HPP

#include <ggraph/operation.hpp>

namespace ggraph {

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

}

#endif
