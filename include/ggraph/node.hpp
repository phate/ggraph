#ifndef NODE_HPP
#define NODE_HPP

#include <ggraph/common.hpp>
#include <ggraph/operation.hpp>

#include <unordered_set>
#include <vector>

namespace ggraph {

class node;

class edge final {
public:
	inline
	edge(node * source, node * sink)
	: sink_(sink)
	, source_(source)
	{}

	edge(const edge &) = delete;

	edge(edge &&) = delete;

	edge &
	operator=(const edge &) = delete;

	edge &
	operator=(edge &&) = delete;

	inline node *
	source() const noexcept
	{
		return source_;
	}

	inline node *
	sink() const noexcept
	{
		return sink_;
	}

private:
	node * sink_;
	node * source_;

	friend node;
};

static inline std::unique_ptr<edge>
create_edge(node * source, node * sink)
{
	return std::make_unique<edge>(source, sink);
}

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
		return inedges_.size();
	}

	inline node *
	predecessor(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < npredecessors());
		return (*std::next(inedges_.begin(), n))->source();
	}

	inline bool
	has_predecessor(node * n) const noexcept
	{
		for (const auto & edge : inedges_) {
			if (edge->source() == n)
				return true;
		}

		return false;
	}

	inline size_t
	nsuccessors() const noexcept
	{
		return outedges_.size();
	}

	inline node *
	successor(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < nsuccessors());
		return outedges_[n]->sink();
	}

	inline bool
	has_successor(node * n) const noexcept
	{
		for (const auto & edge : outedges_) {
			if (edge->sink() == n)
				return true;
		}

		return false;
	}

	inline void
	add_successor(ggraph::node * successor)
	{
		GGRAPH_DEBUG_ASSERT(!has_successor(successor));
		GGRAPH_DEBUG_ASSERT(!successor->has_predecessor(this));
		auto edge = create_edge(this, successor);
		successor->inedges_.insert(edge.get());
		outedges_.push_back(std::move(edge));
	}

	inline void
	remove_successors()
	{
		for (const auto & edge : outedges_)
			edge->sink()->inedges_.erase(edge.get());
		outedges_.clear();
	}

	inline void
	divert_predecessors(ggraph::node * new_successor)
	{
		for (const auto & edge : inedges_) {
			edge->sink_ = new_successor;
			new_successor->inedges_.insert(edge);
		}
		inedges_.clear();
	}

private:
	std::unordered_set<edge*> inedges_;
	std::unique_ptr<ggraph::operation> op_;
	std::vector<std::unique_ptr<edge>> outedges_;
};

}

#endif
