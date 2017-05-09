#include <ggraph/grain.hpp>

namespace ggraph {

grain::~grain()
{}

std::string
grain::debug_string() const
{
	return std::string("grain_") + name;
}

std::unique_ptr<operation>
grain::copy() const
{
	return std::make_unique<grain>(*this);
}

}
