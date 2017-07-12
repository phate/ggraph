#ifndef GGRAPH_ATTRIBUTE_HPP
#define GGRAPH_ATTRIBUTE_HPP

#include <ggraph/common.hpp>

#include <memory>
#include <string>

namespace ggraph {

/* attribute */

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

	virtual std::string
	value_str() const noexcept = 0;

private:
	std::string name_;
};

/* string attribute */

class strattribute final : public attribute {
public:
	virtual
	~strattribute();

	inline
	strattribute(const std::string & name, const std::string & value)
	: attribute(name)
	, value_(value)
	{}

	inline const std::string &
	value() const noexcept
	{
		return value_;
	}

	inline void
	set_value(const std::string & v) noexcept
	{
		value_ = v;
	}

	virtual bool
	operator==(const attribute & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<attribute>
	copy() const override;

	virtual std::string
	value_str() const noexcept override;

private:
	std::string value_;
};

static inline bool
is_strattribute(const attribute & attr) noexcept
{
	return dynamic_cast<const strattribute*>(&attr) != nullptr;
}

static inline std::string
strvalue(const attribute & attribute) noexcept
{
	GGRAPH_DEBUG_ASSERT(is_strattribute(attribute));
	return static_cast<const strattribute*>(&attribute)->value();
}

static inline strattribute &
to_strattribute(attribute & a)
{
	GGRAPH_DEBUG_ASSERT(is_strattribute(a));
	return *static_cast<strattribute*>(&a);
}

static inline const strattribute &
to_strattribute(const attribute & a) noexcept
{
	GGRAPH_DEBUG_ASSERT(is_strattribute(a));
	return *static_cast<const strattribute*>(&a);
}

static inline std::unique_ptr<attribute>
create_strattribute(const std::string & name, const std::string & value)
{
	return std::make_unique<strattribute>(name, value);
}

/* double attribute */

class dblattribute final : public attribute {
public:
	virtual
	~dblattribute();

	inline
	dblattribute(const std::string & name, double value)
	: attribute(name)
	, value_(value)
	{}

	inline double
	value() const noexcept
	{
		return value_;
	}

	inline void
	set_value(double v) noexcept
	{
		value_ = v;
	}

	virtual bool
	operator==(const attribute & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<attribute>
	copy() const override;

	virtual std::string
	value_str() const noexcept override;

private:
	double value_;
};

static inline bool
is_dblattribute(const ggraph::attribute & attribute) noexcept
{
	return dynamic_cast<const dblattribute*>(&attribute) != nullptr;
}

static inline double
dblvalue(const ggraph::attribute & attribute) noexcept
{
	GGRAPH_DEBUG_ASSERT(is_dblattribute(attribute));
	return static_cast<const dblattribute*>(&attribute)->value();
}

static inline const dblattribute &
to_dblattribute(const attribute & a) noexcept
{
	GGRAPH_DEBUG_ASSERT(is_dblattribute(a));
	return *static_cast<const dblattribute*>(&a);
}

static inline dblattribute &
to_dblattribute(attribute & a) noexcept
{
	GGRAPH_DEBUG_ASSERT(is_dblattribute(a));
	return *static_cast<dblattribute*>(&a);
}

static inline std::unique_ptr<attribute>
create_dblattribute(const std::string & name, double value)
{
	return std::make_unique<dblattribute>(name, value);
}

}

#endif
