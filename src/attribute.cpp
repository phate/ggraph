#include <ggraph/attribute.hpp>
#include <ggraph/util/strfmt.hpp>

#include <cmath>
#include <unordered_map>

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

/* nodegraphics attribute */

ngsattribute::~ngsattribute()
{}

bool
ngsattribute::operator==(const attribute & other) const noexcept
{
	/* FIXME: compare open and closed attribute */
	auto a = dynamic_cast<const ngsattribute*>(&other);
	return a && attribute::operator==(other);
}

std::string
ngsattribute::debug_string() const
{
	/* FIXME: */
	return name();
}

std::unique_ptr<attribute>
ngsattribute::copy() const
{
	return std::unique_ptr<attribute>(new ngsattribute(*this));
}

static inline std::string
to_str(bool b)
{
	return b ? "true" : "false";
}

static inline std::string
color(uint32_t c)
{
	static std::unordered_map<size_t, std::string> map({
	  {0, "0"}, {1, "1"}, {2, "2"}, {3, "3"}
	, {4, "4"}, {5, "5"}, {6, "6"}, {7, "7"}
	, {8, "8"}, {9, "9"}, {10, "A"}, {11, "B"}
	, {12, "C"}, {13, "D"}, {14, "E"}, {15, "F"}
	});

	std::string s("#");
	uint32_t mask = 0x00F00000;
	for (size_t n = 0; n < 6; n++) {
		uint32_t v = c & mask;
		GGRAPH_DEBUG_ASSERT(map.find(v) != map.end());
		s += map[v];
		mask = mask >> 4;
	}

	return s;
}

static inline std::string
to_str(const ggraph::graphics & g, bool closed)
{
	return "<y:GroupNode>"
	+ strfmt("<y:Geometry ",
			"height=\"", g.geometry.height,
			"\" width=\"", g.geometry.width,
			"\"/>")
	+ strfmt("<y:Fill ",
			"hasColor=\"", to_str(g.fill.has_color),
			"\" transparent=\"", to_str(g.fill.transparent),
			"\"/>")
	+ strfmt("<y:NodeLabel visible=\"", to_str(g.node_label.visible), "\"/>")
	+ strfmt("<y:BorderStyle ",
			"color=\"", color(g.borderstyle.color),
			"\" type=\"", g.borderstyle.type,
			"\" width=\"", g.borderstyle.width,
			"\"/>")
	+ strfmt("<y:Shape type=\"", g.shape.type, "\"/>")
	+ strfmt("<y:State closed=\"", to_str(closed), "\"/>")
	+ "</y:GroupNode>";
}

std::string
ngsattribute::value_str() const noexcept
{
	return "<y:ProxyAutoBoundsNode><y:Realizers active=\"1\">"
			 + to_str(open(), false)
			 + to_str(closed(), true)
			 + "</y:Realizers></y:ProxyAutoBoundsNode>";
}

}
