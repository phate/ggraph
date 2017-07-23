#include <ggraph/attribute.hpp>
#include <ggraph/util/strfmt.hpp>

#include <cmath>
namespace ggraph {

/* attribute */

attribute::~attribute()
{}

bool
attribute::operator==(const attribute & other) const noexcept
{
	return name() == other.name();
}

std::string
attribute::debug_string() const
{
	return name();
}

/* string attribute */

strattribute::~strattribute()
{}

bool
strattribute::operator==(const attribute & other) const noexcept
{
	auto attr = dynamic_cast<const strattribute*>(&other);
	return attr && value() == attr->value() && attribute::operator==(other);
}

std::string
strattribute::debug_string() const
{
	return name() + ":" + value();
}

std::unique_ptr<attribute>
strattribute::copy() const
{
	return std::unique_ptr<attribute>(new strattribute(*this));
}

std::string
strattribute::value_str() const noexcept
{
	return value_;
}

/* double attribute */

dblattribute::~dblattribute()
{}

bool
dblattribute::operator==(const attribute & other) const noexcept
{
	auto attr = dynamic_cast<const dblattribute*>(&other);
	return attr && value() == attr->value() && attribute::operator==(other);
}

std::string
dblattribute::debug_string() const
{
	return strfmt(name(), ":", value());
}

std::unique_ptr<attribute>
dblattribute::copy() const
{
	return std::unique_ptr<attribute>(new dblattribute(*this));
}

std::string
dblattribute::value_str() const noexcept
{
	return std::isnan(value()) ? "0.0" : strfmt(value());
}

}
