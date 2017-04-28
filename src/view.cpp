#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/join.hpp>
#include <ggraph/node.hpp>
#include <ggraph/util/strfmt.hpp>
#include <ggraph/view.hpp>

#include <typeindex>
#include <unordered_map>

namespace ggraph {

static inline std::string
node_shape(const node * n)
{
	static std::unordered_map<std::type_index, std::string> shape_map({
		{std::type_index(typeid(ggraph::grain)), "box"}
	,	{std::type_index(typeid(ggraph::fork)), "triangle"}
	,	{std::type_index(typeid(ggraph::join)), "invtriangle"}
	});

	auto it = shape_map.find(std::type_index(typeid(n->operation())));
	if (it != shape_map.end())
		return it->second;

	return "circle";
}

static inline std::string
emit_node(const node * n)
{
	return strfmt(
		(uintptr_t)n,
		"[label=",
		n->operation().debug_string(),
		" shape=",
		node_shape(n),
		"]");
}

static inline std::string
emit_edge(const node * n1, const node * n2)
{
	return strfmt((uintptr_t)n1, " -- ", (uintptr_t)n2);
}

static inline std::string
to_dot(const node * n, std::unordered_set<const node*> & visited)
{
	std::string dot;
	dot.append(strfmt(emit_node(n), "\n"));

	for (size_t s = 0; s < n->nsuccessors(); s++)
		dot.append(strfmt(emit_edge(n, n->successor(s)), "\n"));

	visited.insert(n);
	for (size_t s = 0; s < n->nsuccessors(); s++) {
		auto successor = n->successor(s);
		if (visited.find(successor) == visited.end())
			dot.append(to_dot(successor, visited));
	}

	return dot;
}

std::string
to_dot(const graph & g)
{
	std::unordered_set<const node*> visited;

	std::string dot("graph ggraph {");
	dot.append(to_dot(g.entry(), visited));
	dot.append("}\n");
	return dot;
}

void
view(const graph & g, FILE * out)
{
	fputs(to_dot(g).c_str(), out);
	fflush(out);
}

}
