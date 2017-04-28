#include <ggraph/join.hpp>

namespace ggraph {

join::~join()
{}

std::string
join::debug_string() const
{
	return "join";
}

std::unique_ptr<operation>
join::copy() const
{
	return std::make_unique<join>(*this);
}

}
