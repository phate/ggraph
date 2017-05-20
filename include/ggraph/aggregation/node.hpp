#ifndef GGRAPH_AGGREGATION_NODE_HPP
#define GGRAPH_AGGREGATION_NODE_HPP

#include <ggraph/aggregation/type.hpp>
#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ggraph {
namespace agg {

class node final {
public:
	inline
	~node()
	{}

	inline
	node(std::unique_ptr<ggraph::agg::type> type) noexcept
	: parent_(nullptr)
	, type_(std::move(type))
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

	inline const ggraph::agg::type &
	type() const noexcept
	{
		return *type_;
	}

	inline void
	add_child(std::unique_ptr<node> n)
	{
		GGRAPH_DEBUG_ASSERT(n->parent() == nullptr);

		children_.emplace_back(std::move(n));
		children_.back()->parent_ = this;
	}

private:
	const node * parent_;
	std::unique_ptr<ggraph::agg::type> type_;
	std::vector<std::unique_ptr<node>> children_;
};

static inline std::unique_ptr<node>
create_forkjoin_node(const ggraph::fork & fork, const ggraph::join & join)
{
	return std::make_unique<node>(std::make_unique<forkjoin_type>(fork, join));
}

static inline std::unique_ptr<node>
create_linear_node(std::unique_ptr<node> n1, std::unique_ptr<node> n2)
{
	std::unique_ptr<node> ln(new node(std::make_unique<linear_type>()));
	ln->add_child(std::move(n1));
	ln->add_child(std::move(n2));
	return ln;
}

static inline std::unique_ptr<node>
create_grain_node(const ggraph::grain & grain)
{
	return std::make_unique<node>(std::make_unique<grain_type>(grain));
}

}}

#endif
