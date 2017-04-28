#include <ggraph/graph.hpp>

namespace ggraph {

/* entry */

entry::~entry()
{}

std::string
entry::debug_string() const
{
	return "entry";
}

std::unique_ptr<operation>
entry::copy() const
{
	return std::make_unique<entry>(*this);
}

/* exit */

exit::~exit()
{}

std::string
exit::debug_string() const
{
	return "exit";
}

std::unique_ptr<operation>
exit::copy() const
{
	return std::make_unique<exit>(*this);
}

}
