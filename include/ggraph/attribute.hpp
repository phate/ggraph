#ifndef GGRAPH_ATTRIBUTE_HPP
#define GGRAPH_ATTRIBUTE_HPP

#include <memory>
#include <string>

namespace ggraph {

class attribute {
public:
	virtual
	~attribute();

	inline
	attribute(const std::string & name)
	: name_(name)
	{}

	inline const std::string &
	name() const noexcept
	{
		return name_;
	}

	virtual bool
	operator==(const attribute & other) const noexcept;

	bool
	operator!=(const attribute & other) const noexcept
	{
		return !(*this == other);
	}

	virtual std::string
	debug_string() const;

	virtual std::unique_ptr<attribute>
	copy() const = 0;

private:
	std::string name_;
};

class string_attribute final : public attribute {
public:
	virtual
	~string_attribute();

	inline
	string_attribute(const std::string & name, const std::string & value)
	: attribute(name)
	, value_(value)
	{}

	inline const std::string &
	value() const noexcept
	{
		return value_;
	}

	virtual bool
	operator==(const attribute & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<attribute>
	copy() const override;

private:
	std::string value_;
};

static inline bool
is_string_attribute(const attribute & attr) noexcept
{
	return dynamic_cast<const string_attribute*>(&attr) != nullptr;
}

static inline std::unique_ptr<attribute>
create_string_attribute(const std::string & name, const std::string & value)
{
	return std::make_unique<string_attribute>(name, value);
}

}

#endif
