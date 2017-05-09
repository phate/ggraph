#include <ggraph/fork.hpp>

namespace ggraph {

fork::~fork()
{}

std::string
fork::debug_string() const
{
	return std::string("fork_") + name;
}

std::unique_ptr<operation>
fork::copy() const
{
	return std::make_unique<fork>(*this);
}

}
