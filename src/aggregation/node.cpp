#include <ggraph/aggregation/node.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

/* node */

node::~node()
{}

/* forkjoin_node */

std::string
forkjoin_node::debug_string() const
{
	return strfmt(fork().debug_string(), "/", join().debug_string());
}

/* linear_node */

std::string
linear_node::debug_string() const
{
	return "linear";
}

/* grain_node */

std::string
grain_node::debug_string() const
{
	return grain().debug_string();
}

}}
