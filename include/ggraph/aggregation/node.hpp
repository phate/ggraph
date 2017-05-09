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
	{}

	node(const node &) = delete;

	inline
	node(node && other)
	{
		children_.clear();
		for (auto & child : children_)
			children_.emplace_back(std::move(child));
		other.children_.clear();
	}

	node &
	operator=(const node &) = delete;

	node &
	operator=(node && other)
	{
		if (this == &other)
			return *this;

		children_.clear();
		for (auto & child : children_)
			children_.emplace_back(std::move(child));
		other.children_.clear();

		return *this;
	}

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

	virtual std::string
	debug_string() const = 0;

protected:
	inline void
	add_child(std::unique_ptr<node> n)
	{
		children_.emplace_back(std::move(n));
	}

private:
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

	inline
	forkjoin_node(forkjoin_node && other)
	: node(std::move(other))
	, fork_(std::move(other.fork_))
	, join_(std::move(other.join_))
	{}

	forkjoin_node &
	operator=(const forkjoin_node &) = delete;

	forkjoin_node &
	operator=(forkjoin_node && other)
	{
		if (this == &other)
			return *this;

		node::operator=(std::move(other));
		fork_ = std::move(other.fork_);
		join_ = std::move(other.join_);

		return *this;
	}

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

	inline
	linear_node(linear_node && other)
	: node(std::move(other))
	{}

	linear_node &
	operator=(const linear_node &) = delete;

	linear_node &
	operator=(linear_node && other)
	{
		return *static_cast<linear_node*>(&node::operator=(std::move(other)));
	}

	virtual std::string
	debug_string() const override;
};

class grain_node final : public node {
public:
	inline
	grain_node(const ggraph::grain & grain)
	: node()
	, grain_(grain)
	{}

	grain_node(const grain_node &) = delete;

	inline
	grain_node(grain_node && other)
	: node(std::move(other))
	, grain_(std::move(other.grain_))
	{}

	grain_node &
	operator=(const grain_node &) = delete;

	grain_node &
	operator=(grain_node && other)
	{
		if (this == &other)
			return *this;

		node::operator=(std::move(other));
		grain_ = std::move(other.grain_);
		return *this;
	}

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

}}

#endif