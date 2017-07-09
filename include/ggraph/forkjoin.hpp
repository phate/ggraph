#ifndef GGRAPH_AGGREGATION_FORKJOIN_HPP
#define GGRAPH_AGGREGATION_FORKJOIN_HPP

#include <ggraph/fork.hpp>
#include <ggraph/join.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {
namespace agg {

class forkjoin final : public operation {
public:
	virtual
	~forkjoin();

	inline
	forkjoin(const ggraph::fork & fork, const ggraph::join & join)
	: operation({})
	, fork_(fork)
	, join_(join)
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;

	inline const ggraph::fork &
	fork() const noexcept
	{
		return fork_;
	}

	inline const ggraph::join &
	join() const noexcept
	{
		return join_;
	}

private:
	ggraph::fork fork_;
	ggraph::join join_;
};

static inline bool
is_forkjoin(const ggraph::operation & operation) noexcept
{
	return dynamic_cast<const forkjoin*>(&operation) != nullptr;
}

}}

#endif
