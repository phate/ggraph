#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/type.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

/* type */

type::~type()
{}

/* forkjoin operation */

forkjoin::~forkjoin()
{}

std::string
forkjoin::debug_string() const
{
	return strfmt(fork().debug_string(), "/", join().debug_string());
}

std::unique_ptr<operation>
forkjoin::copy() const
{
	return std::unique_ptr<forkjoin>(new forkjoin(*this));
}

/* forkjoin type */

std::string
forkjoin_type::debug_string() const
{
	return strfmt(fork().debug_string(), "/", join().debug_string());
}

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

/* linear type */

std::string
linear_type::debug_string() const
{
	return "linear";
}

/* grain type */

std::string
grain_type::debug_string() const
{
	return grain().debug_string();
}

}}
