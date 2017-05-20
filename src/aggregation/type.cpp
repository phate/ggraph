#include <ggraph/aggregation/type.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

/* type */

type::~type()
{}

/* forkjoin type */

std::string
forkjoin_type::debug_string() const
{
	return strfmt(fork().debug_string(), "/", join().debug_string());
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
