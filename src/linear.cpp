#include <ggraph/linear.hpp>

namespace ggraph {
namespace agg {

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

