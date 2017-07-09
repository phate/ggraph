#include <ggraph/forkjoin.hpp>
#include <ggraph/util/strfmt.hpp>

namespace ggraph {
namespace agg {

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

}}
