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

/* type */

class type {
public:
	virtual
	~type();

	inline constexpr
	type()
	{}

	virtual std::string
	debug_string() const = 0;
};

/* forkjoin type */

class forkjoin_type final : public type {
public:
	inline
	forkjoin_type(const ggraph::fork & fork, const ggraph::join & join)
	: type()
	, fork_(fork)
	, join_(join)
	{}

	virtual std::string
	debug_string() const override;

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
is_forkjoin_type(const type & t) noexcept
{
	return dynamic_cast<const forkjoin_type*>(&t) != nullptr;
}

/* linear type */

class linear_type final : public type {
public:
	inline
	linear_type()
	: type()
	{}

	virtual std::string
	debug_string() const override;
};

static inline bool
is_linear_type(const type & t) noexcept
{
	return dynamic_cast<const linear_type*>(&t) != nullptr;
}

/* grain type */

class grain_type final : public type {
public:
	inline
	grain_type(const ggraph::grain & grain)
	: type()
	, grain_(grain)
	{}

	inline const ggraph::grain &
	grain() const noexcept
	{
		return grain_;
	}

	virtual std::string
	debug_string() const override;

private:
	ggraph::grain grain_;
};

static inline bool
is_grain_type(const type & t) noexcept
{
	return dynamic_cast<const grain_type*>(&t) != nullptr;
}

}}

#endif
