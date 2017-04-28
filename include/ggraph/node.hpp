#ifndef NODE_HPP
#define NODE_HPP

#include <ggraph/common.hpp>
#include <ggraph/operation.hpp>

#include <unordered_set>

namespace ggraph {

class node final {
public:
	virtual
	~node();

	inline
	node(ggraph::operation & op)
	: op_(std::move(op.copy()))
	{}

	node(const node &) = delete;

	node(node &&) = delete;

	node &
	operator=(const node &) = delete;

	node &
	operator=(node &&) = delete;

	const ggraph::operation &
	operation() const noexcept
	{
		return *op_;
	}

	inline size_t
	npredecessors() const noexcept
	{
		return predecessors_.size();
	}

	inline node *
	predecessor(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < npredecessors());
		return *std::next(predecessors_.begin(), n);
	}

	inline bool
	has_predecessor(node * n) const noexcept
	{
		return predecessors_.find(n) != predecessors_.end();
	}

	inline size_t
	nsuccessors() const noexcept
	{
		return successors_.size();
	}

	inline node *
	successor(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < nsuccessors());
		return *std::next(successors_.begin(), n);
	}

	inline bool
	has_successor(node * n) const noexcept
	{
		return successors_.find(n) != successors_.end();
	}

	inline void
	add_successor(ggraph::node * successor)
	{
		GGRAPH_DEBUG_ASSERT(!has_successor(successor));
		GGRAPH_DEBUG_ASSERT(!successor->has_predecessor(this));
		successor->predecessors_.insert(this);
		successors_.insert(successor);
	}

private:
	std::unique_ptr<ggraph::operation> op_;
	std::unordered_set<node*> successors_;
	std::unordered_set<node*> predecessors_;
};

}

#endif
