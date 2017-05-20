#ifndef GGRAPH_AGGREGATION_NODE_HPP
#define GGRAPH_AGGREGATION_NODE_HPP

#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ggraph {
namespace agg {

class node {
public:
	virtual
	~node();

	inline
	node() noexcept
	: parent_(nullptr)
	{}

	node(const node &) = delete;

	node(node && other) = delete;

	node &
	operator=(const node &) = delete;

	node &
	operator=(node && other) = delete;

	inline size_t
	nchildren() const noexcept
	{
		return children_.size();
	}

	inline const node *
	child(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < nchildren());
		return children_[n].get();
	}

	inline const node *
	parent() const noexcept
	{
		return parent_;
	}

	inline bool
	is_ancestor(const node * n) const noexcept
	{
		auto p = parent();
		while (p) {
			if (p == n)
				return true;

			p = p->parent();
		}

		return false;
	}

	virtual std::string
	debug_string() const = 0;

protected:
	inline void
	add_child(std::unique_ptr<node> n)
	{
		GGRAPH_DEBUG_ASSERT(n->parent() == nullptr);

		children_.emplace_back(std::move(n));
		children_.back()->parent_ = this;
	}

private:
	const node * parent_;
	std::vector<std::unique_ptr<node>> children_;
};

class forkjoin_node final : public node {
public:
	inline
	forkjoin_node(const ggraph::fork & fork, const ggraph::join & join)
	: fork_(fork)
	, join_(join)
	{}

	forkjoin_node(const forkjoin_node &) = delete;

	forkjoin_node(forkjoin_node && other) = delete;

	forkjoin_node &
	operator=(const forkjoin_node &) = delete;

	forkjoin_node &
	operator=(forkjoin_node && other) = delete;

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

	inline void
	add_sibling(std::unique_ptr<node> n)
	{
		add_child(std::move(n));
	}

private:
	ggraph::fork fork_;
	ggraph::join join_;
};

static inline bool
is_forkjoin_node(const node * n) noexcept
{
	return dynamic_cast<const forkjoin_node*>(n) != nullptr;
}

class linear_node final : public node {
public:
	inline
	linear_node(std::unique_ptr<node> n1, std::unique_ptr<node> n2)
	: node()
	{
		add_child(std::move(n1));
		add_child(std::move(n2));
	}

	linear_node(const linear_node &) = delete;

	linear_node(linear_node && other) = delete;

	linear_node &
	operator=(const linear_node &) = delete;

	linear_node &
	operator=(linear_node && other) = delete;

	virtual std::string
	debug_string() const override;
};

static inline bool
is_linear_node(const node * n) noexcept
{
	return dynamic_cast<const linear_node*>(n) != nullptr;
}

class grain_node final : public node {
public:
	inline
	grain_node(const ggraph::grain & grain)
	: node()
	, grain_(grain)
	{}

	grain_node(const grain_node &) = delete;

	grain_node(grain_node && other) = delete;

	grain_node &
	operator=(const grain_node &) = delete;

	grain_node &
	operator=(grain_node && other) = delete;

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
is_grain_node(const node * n) noexcept
{
	return dynamic_cast<const grain_node*>(n) != nullptr;
}

}}

#endif
