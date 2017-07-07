#ifndef GGRAPH_OPERATION_HPP
#define GGRAPH_OPERATION_HPP

#include <ggraph/attribute.hpp>

#include <memory>
#include <string>
#include <unordered_set>

namespace ggraph {

class operation {
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

	virtual std::string
	debug_string() const = 0;

	virtual std::unique_ptr<operation>
	copy() const = 0;

private:
	std::unordered_set<std::unique_ptr<attribute>> attributes_;
};

}

#endif
