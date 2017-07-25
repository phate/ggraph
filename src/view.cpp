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
emit_attributes(const operation & op)
{
	std::string str;
	for (const auto & attribute : op) {
		str += strfmt(attribute.name(), "=", attribute.value_str()) + "\\n";
	}

	return str;
}

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
		"[label=\"",
		n->operation().debug_string(), "\\n", emit_attributes(n->operation()),
		"\" shape=",
		node_shape(n),
		"]");
}

static inline std::string
emit_edge(const node * n1, const node * n2)
{
	return strfmt((uintptr_t)n1, " -- ", (uintptr_t)n2);
}

std::string
to_dot(const graph & g)
{
	std::unordered_set<const node*> visited;

	std::string dot("graph ggraph {");

	for (const auto & node : g)
		dot.append(strfmt(emit_node(&node), "\n"));

	for (const auto & node : g) {
		for (size_t s = 0; s < node.nsuccessors(); s++)
			dot.append(strfmt(emit_edge(&node, node.successor(s)), "\n"));
	}

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
