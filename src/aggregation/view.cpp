#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/view.hpp>

namespace ggraph {
namespace agg {

std::string
to_str(const node & n)
{
	std::function<std::string(const node & n, size_t)> f = [&] (
		const node & n,
		size_t depth
	) {
		std::string subtree(depth, '-');
		subtree += n.debug_string() + "\n";

		for (size_t c = 0; c < n.nchildren(); c++)
			subtree += f(*n.child(c), depth+1);

		return subtree;
	};

	return f(n, 0);
}

void
view(const node & n, FILE * out)
{
	fputs(to_str(n).c_str(), out);
	fflush(out);
}

}}
