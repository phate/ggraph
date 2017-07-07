#ifndef GGRAPH_OPERATION_HPP
#define GGRAPH_OPERATION_HPP

#include <ggraph/attribute.hpp>

#include <memory>
#include <string>
#include <unordered_set>

namespace ggraph {

class operation {
	class const_iterator final {
	public:
		inline
		const_iterator(const std::unordered_set<std::unique_ptr<attribute>>::const_iterator & it)
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

		inline const const_iterator &
		operator++() noexcept
		{
			it_++;
			return *this;
		}

		inline const_iterator
		operator++(int) noexcept
		{
			auto tmp = it_;
			it_++;
			return tmp;
		}

		inline const attribute *
		operator->() const noexcept
		{
			return (*it_).get();
		}

		inline const attribute &
		operator*() const noexcept
		{
			return *(*it_).get();
		}

	private:
		std::unordered_set<std::unique_ptr<attribute>>::const_iterator it_;
	};

public:
	virtual
	~operation();

	inline
	operation(std::unordered_set<std::unique_ptr<attribute>> attributes)
	: attributes_(std::move(attributes))
	{}

	inline
	operation(const operation & other)
	{
		for (const auto & attribute: other.attributes_)
			attributes_.insert(std::move(attribute->copy()));
	}

	inline
	operation(operation && other)
	: attributes_(std::move(other.attributes_))
	{}

	operation &
	operator=(const operation & other)
	{
		if (this == &other)
			return *this;

		for (const auto & attribute : other.attributes_)
			attributes_.insert(std::move(attribute->copy()));

		return *this;
	}

	operation &
	operator=(operation && other)
	{
		if (this == &other)
			return *this;

		attributes_ = std::move(other.attributes_);
	}

	inline const_iterator
	begin() const
	{
		return const_iterator(attributes_.begin());
	}

	inline const_iterator
	end() const
	{
		return const_iterator(attributes_.end());
	}

	virtual std::string
	debug_string() const = 0;

	virtual std::unique_ptr<operation>
	copy() const = 0;

private:
	std::unordered_set<std::unique_ptr<attribute>> attributes_;
};

}

#endif
