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
	{
		while (nchildren())
			first_child()->detach();
	}

	inline
	node(std::unique_ptr<ggraph::agg::type> type) noexcept
	: parent_(nullptr)
	, nchildren_(0)
	, type_(std::move(type))
	{
		sibling.prev = sibling.next = nullptr;
		children.first = children.last = nullptr;
	}

	node(const node &) = delete;

	node(node && other) = delete;

	node &
	operator=(const node &) = delete;

	node &
	operator=(node && other) = delete;

	inline size_t
	nchildren() const noexcept
	{
		return nchildren_;
	}

	inline const node *
	child(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < nchildren());
		size_t tmp = 0;
		auto node  = children.first;
		while (tmp != n) {
			tmp++;
			node = node->sibling.next;
		}

		return node;
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

	inline ggraph::agg::node *
	first_child() noexcept
	{
		return children.first;
	}

	inline ggraph::agg::node *
	last_child() noexcept
	{
		return children.last;
	}

	inline ggraph::agg::node *
	prev_sibling() noexcept
	{
		return sibling.prev;
	}

	inline ggraph::agg::node *
	next_sibling() noexcept
	{
		return sibling.next;
	}

	inline void
	add_child(std::unique_ptr<node> n)
	{
		GGRAPH_DEBUG_ASSERT(n->parent() == nullptr);
		auto child = n.release();

		nchildren_++;
		child->parent_ = this;
		if (!children.last) {
			GGRAPH_DEBUG_ASSERT(children.first == nullptr);
			children.first = children.last = child;
			child->sibling.prev = child->sibling.next = nullptr;
		} else {
			child->sibling.next = nullptr;
			child->sibling.prev = children.last;
			children.last->sibling.next = child;
			children.last = child;
		}
	}

	inline std::unique_ptr<node>
	detach()
	{
		parent_->nchildren_--;
		if (parent_->children.first == this)
			parent_->children.first = sibling.next;
		if (parent_->children.last == this)
			parent_->children.last = sibling.prev;
		parent_ = nullptr;

		if (sibling.prev)
			sibling.prev->sibling.next = sibling.next;
		if (sibling.next)
			sibling.next->sibling.prev = sibling.prev;
		sibling.prev = sibling.next = nullptr;
		return std::unique_ptr<node>(this);
	}

private:
	struct {
		node * first;
		node * last;
	} children;

	struct {
		node * prev;
		node * next;
	} sibling;

	node * parent_;
	size_t nchildren_;
	std::unique_ptr<ggraph::agg::type> type_;
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
