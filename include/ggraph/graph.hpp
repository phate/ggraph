#ifndef GGRAPH_GRAPH_HPP
#define GGRAPH_GRAPH_HPP

#include <ggraph/node.hpp>
#include <ggraph/operation.hpp>

#include <memory>
#include <unordered_set>

namespace ggraph {

/* entry node */

class entry final : public operation {
public:
	virtual
	~entry();

	inline
	entry()
	: operation({})
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_entry(const ggraph::node * n) noexcept
{
	return dynamic_cast<const ggraph::entry*>(&n->operation()) != nullptr;
}

/* exit node */

class exit final : public operation {
public:
	virtual
	~exit();

	inline
	exit()
	: operation({})
	{}

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;
};

static inline bool
is_exit(const ggraph::node * n) noexcept
{
	return dynamic_cast<const ggraph::exit*>(&n->operation()) != nullptr;
}

class graph final {
	class iterator final {
	public:
		inline
		iterator(const std::unordered_set<std::unique_ptr<ggraph::node>>::iterator & it)
		: it_(it)
		{}

		inline bool
		operator==(const iterator & other) const noexcept
		{
			return it_ == other.it_;
		}

		inline bool
		operator!=(const iterator & other) const noexcept
		{
			return !(*this == other);
		}

		inline iterator &
		operator++() noexcept
		{
			it_++;
			return *this;
		}

		inline iterator
		operator++(int) noexcept
		{
			auto tmp = *this;
			it_++;
			return tmp;
		}

		inline ggraph::node &
		operator*() const
		{
			return *it_->get();
		}

		inline ggraph::node *
		operator->() const
		{
			return it_->get();
		}

	private:
		std::unordered_set<std::unique_ptr<ggraph::node>>::iterator it_;
	};

	class const_iterator final {
	public:
		inline
		const_iterator(const std::unordered_set<std::unique_ptr<ggraph::node>>::const_iterator & it)
		: it_(it)
		{}

		inline bool
		operator==(const const_iterator & other) const noexcept
		{
			return it_ == other.it_;
		}

		inline bool
		operator!=(const const_iterator & other) const noexcept
		{
			return !(*this == other);
		}

		inline const_iterator &
		operator++() noexcept
		{
			it_++;
			return *this;
		}

		inline const_iterator
		operator++(int) noexcept
		{
			auto tmp = *this;
			it_++;
			return tmp;
		}

		inline const ggraph::node &
		operator*() const
		{
			return *it_->get();
		}

		inline const ggraph::node *
		operator->() const
		{
			return it_->get();
		}

	private:
		std::unordered_set<std::unique_ptr<ggraph::node>>::const_iterator it_;
	};

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

	inline iterator
	begin()
	{
		return iterator(nodes_.begin());
	}

	inline const_iterator
	begin() const
	{
		return const_iterator(nodes_.begin());
	}

	inline iterator
	end()
	{
		return iterator(nodes_.end());
	}

	inline const_iterator
	end() const
	{
		return const_iterator(nodes_.end());
	}

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

bool
is_valid(const ggraph::graph & g);

}

#endif
