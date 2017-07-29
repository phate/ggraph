#ifndef GGRAPH_SIBLING_HPP
#define GGRAPH_SIBLING_HPP

#include <ggraph/operation.hpp>

namespace ggraph {

class sibling final : public operation {
public:
	virtual
	~sibling();

	sibling();

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_sibling(const operation & op) noexcept
{
	return dynamic_cast<const sibling*>(&op) != nullptr;
}

}

#endif
