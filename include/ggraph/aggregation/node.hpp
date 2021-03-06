#ifndef GGRAPH_AGGREGATION_NODE_HPP
#define GGRAPH_AGGREGATION_NODE_HPP

#include <ggraph/fork.hpp>
#include <ggraph/forkjoin.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/linear.hpp>
#include <ggraph/sibling.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ggraph {
namespace agg {

class node final {
	class const_iterator final {
	public:
		inline
		const_iterator(ggraph::agg::node * n)
		: node_(n)
		{}

		inline const ggraph::agg::node *
		node() const noexcept
		{
			return node_;
		}

		inline const const_iterator &
		operator++() noexcept
		{
			node_ = node_->next_sibling();
			return *this;
		}

		inline const_iterator
		operator++(int) noexcept
		{
			auto node = node_;
			node_ = node_->next_sibling();
			return const_iterator(node);
		}

		inline bool
		operator==(const const_iterator & other) const noexcept
		{
			return node_ == other.node_;
		}

		inline bool
		operator!=(const const_iterator & other) const noexcept
		{
			return !(*this == other);
		}

		inline const ggraph::agg::node &
		operator*() const noexcept
		{
			return *node_;
		}

		inline const ggraph::agg::node *
		operator->() const noexcept
		{
			return node_;
		}

	private:
		ggraph::agg::node * node_;
	};

public:
	inline
	~node()
	{
		while (nchildren())
			first_child()->detach();
	}

	inline
	node(std::unique_ptr<ggraph::operation> operation) noexcept
	: parent_(nullptr)
	, nchildren_(0)
	, operation_(std::move(operation))
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

	inline const_iterator
	begin() const noexcept
	{
		return const_iterator(children.first);
	}

	inline const_iterator
	end() const noexcept
	{
		return const_iterator(nullptr);
	}

	inline size_t
	nchildren() const noexcept
	{
		return nchildren_;
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

	inline const ggraph::operation &
	operation() const noexcept
	{
		return *operation_;
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

	inline node *
	add_next_sibling(std::unique_ptr<node> n)
	{
		GGRAPH_DEBUG_ASSERT(parent() != nullptr);
		GGRAPH_DEBUG_ASSERT(n->parent() == nullptr);
		auto ns = n.release();

		parent_->nchildren_++;
		ns->parent_ = parent_;
		ns->sibling.prev = this;
		ns->sibling.next = sibling.next;
		sibling.next = ns;
		if (ns->sibling.next)
			ns->sibling.next->sibling.prev = ns;
		else
			parent_->children.last = ns;

		return ns;
	}

	inline node *
	add_prev_sibling(std::unique_ptr<node> n)
	{
		GGRAPH_DEBUG_ASSERT(parent() != nullptr);
		GGRAPH_DEBUG_ASSERT(n->parent() == nullptr);
		auto ps = n.release();

		parent_->nchildren_++;
		ps->parent_ = parent_;
		ps->sibling.prev = sibling.prev;
		ps->sibling.next = this;
		sibling.prev = ps;
		if (ps->sibling.prev)
			ps->sibling.prev->sibling.next = ps;
		else
			parent_->children.first = ps;

		return ps;
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
	std::unique_ptr<ggraph::operation> operation_;
};

#define ITERATE_CHILDREN_SAFE(node, child, next) \
	for (child = (node)->first_child(), next = child ? child->next_sibling() : nullptr; \
		child != nullptr; child = next, next = next ? next->next_sibling() : nullptr)

static inline std::unique_ptr<node>
create_forkjoin_node(const ggraph::fork & fork, const ggraph::join & join)
{
	return std::make_unique<node>(std::make_unique<forkjoin>(fork, join));
}

static inline std::unique_ptr<node>
create_linear_node(std::unique_ptr<node> n1, std::unique_ptr<node> n2)
{
	std::unique_ptr<node> ln(new node(std::make_unique<linear>()));
	ln->add_child(std::move(n1));
	ln->add_child(std::move(n2));
	return ln;
}

static inline std::unique_ptr<node>
create_sibling_node()
{
	return std::make_unique<node>(std::make_unique<sibling>());
}

static inline std::unique_ptr<node>
create_grain_node(const ggraph::grain & grain)
{
	return std::make_unique<node>(grain.copy());
}

void
propagate(const ggraph::agg::node & n);

void
segregate(ggraph::agg::node & n);

size_t
theta(const ggraph::agg::node & n);

size_t
max_open_nodes(const ggraph::agg::node & n);

void
normalize(ggraph::agg::node & n);

void
prune(ggraph::agg::node & n);

}}

#endif
