#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/type.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

/* linear operation */

linear::~linear()
{}

std::string
linear::debug_string() const
{
	return "linear";
}

std::unique_ptr<operation>
linear::copy() const
{
	return std::unique_ptr<linear>(new linear(*this));
}

}}
