#include <ggraph/attribute.hpp>
#include <ggraph/util/strfmt.hpp>

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
	return value() == attr->value() && attribute::operator==(other);
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

/* double attribute */

dblattribute::~dblattribute()
{}

bool
dblattribute::operator==(const attribute & other) const noexcept
{
	auto attr = dynamic_cast<const dblattribute*>(&other);
	return value() == attr->value() && attribute::operator==(other);
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

}
