#ifndef GGRAPH_GRAPH_HPP
#define GGRAPH_GRAPH_HPP

#include <ggraph/node.hpp>
#include <ggraph/operation.hpp>

#include <memory>
#include <unordered_set>

namespace ggraph {

class entry final : public operation {
public:
	virtual
	~entry();

	inline constexpr
	entry()
	: operation()
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

class exit final : public operation {
public:
	virtual
	~exit();

	inline constexpr
	exit()
	: operation()
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

class graph final {
public:
	inline
	~graph()
	{}

	inline
	graph()
	{
		ggraph::exit x;
		ggraph::entry e;
		exit_ = add_node(x, {});
		entry_ = add_node(e, {});
	}

	graph(const graph &) = delete;

	graph(const graph &&) = delete;

	graph &
	operator=(const graph &) = delete;

	graph &
	operator=(graph &&) = delete;

	inline ggraph::node *
	entry() const noexcept
	{
		return entry_;
	}

	inline ggraph::node *
	exit() const noexcept
	{
		return exit_;
	}

	inline size_t
	nnodes() const noexcept
	{
		return nodes_.size();
	}

	inline ggraph::node *
	node(size_t n) const noexcept
	{
		GGRAPH_DEBUG_ASSERT(n < nnodes());
		return std::next(nodes_.begin(), n)->get();
	}

	ggraph::node *
	add_node(operation & op, const std::unordered_set<ggraph::node*> & predecessors)
	{
		auto n = std::make_unique<ggraph::node>(op);
		for (const auto & predecessor : predecessors)
			predecessor->add_successor(n.get());

		auto ptr = n.get();
		nodes_.insert(std::move(n));
		return ptr;
	}

private:
	ggraph::node * exit_;
	ggraph::node * entry_;
	std::unordered_set<std::unique_ptr<ggraph::node>> nodes_;
};

}

#endif
