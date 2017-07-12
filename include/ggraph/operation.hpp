#ifndef GGRAPH_OPERATION_HPP
#define GGRAPH_OPERATION_HPP

#include <ggraph/attribute.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ggraph {

class operation {
	class const_iterator final {
	public:
		inline
		const_iterator(
			const std::unordered_map<std::string, std::unique_ptr<attribute>>::const_iterator & it)
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
			return it_->second.get();
		}

		inline const attribute &
		operator*() const noexcept
		{
			return *it_->second;
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<attribute>>::const_iterator it_;
	};

public:
	virtual
	~operation();

	inline
	operation(std::unordered_set<std::unique_ptr<attribute>> attributes)
	{
		for (const auto & attribute : attributes) {
			GGRAPH_DEBUG_ASSERT(attributes_.find(attribute->name()) == attributes_.end());
			/* FIXME: I would not like to copy the attribute but move it instead */
			attributes_[attribute->name()] = std::move(attribute->copy());
		}
	}

	inline
	operation(const operation & other)
	{
		for (const auto & pair : other.attributes_) {
			GGRAPH_DEBUG_ASSERT(attributes_.find(pair.second->name()) == attributes_.end());
			attributes_[pair.second->name()] = std::move(pair.second->copy());
		}
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

		attributes_.clear();
		for (const auto & pair : other.attributes_)
			attributes_[pair.second->name()] = std::move(pair.second->copy());

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

	inline attribute *
	find(const std::string & name) const noexcept
	{
		auto it = attributes_.find(name);
		return it != attributes_.end() ? it->second.get() : nullptr;
	}

	virtual std::string
	debug_string() const = 0;

	virtual std::unique_ptr<operation>
	copy() const = 0;

private:
	std::unordered_map<std::string, std::unique_ptr<attribute>> attributes_;
};

}

#endif
