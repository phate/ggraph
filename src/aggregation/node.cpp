#include <ggraph/aggregation/node.hpp>

namespace ggraph {
namespace agg {

void
prune(ggraph::agg::node & n)
{
	auto child = n.first_child();
	while (child) {
		auto next = child->next_sibling();
		prune(*child);
		child = next;
	}

	if (n.nchildren() == 0 && !is_grain_type(n.type()))
		n.detach();
}

}}
