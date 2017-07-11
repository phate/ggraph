#ifndef GGRAPH_ATTRIBUTE_HPP
#define GGRAPH_ATTRIBUTE_HPP

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
is_strattribute(const attribute & attr) noexcept
{
	return dynamic_cast<const strattribute*>(&attr) != nullptr;
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

	virtual bool
	operator==(const attribute & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<attribute>
	copy() const override;

private:
	double value_;
};

static inline bool
is_dblattribute(const ggraph::attribute & attribute) noexcept
{
	return dynamic_cast<const dblattribute*>(&attribute) != nullptr;
}

static inline std::unique_ptr<attribute>
create_dblattribute(const std::string & name, double value)
{
	return std::make_unique<dblattribute>(name, value);
}

}

#endif
