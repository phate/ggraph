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
