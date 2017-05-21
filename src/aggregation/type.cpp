#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/type.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

/* type */

type::~type()
{}

bool
type::reduce(node & n) const
{
	return false;
}

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
