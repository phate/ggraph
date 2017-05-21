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

void
normalize(ggraph::agg::node & n)
{
	std::function<void(ggraph::agg::node &)> reduce = [&](ggraph::agg::node & n)
	{
		auto child = n.first_child();
		while (child) {
			reduce(*child);
			child = child->next_sibling();
		}

		n.type().reduce(n);
	};

	reduce(n);
	prune(n);
}

}}
