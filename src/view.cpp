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
emit_string_attribute(const ggraph::attribute & attribute)
{
	GGRAPH_DEBUG_ASSERT(is_string_attribute(attribute));
	auto & sa = *static_cast<const string_attribute*>(&attribute);
	return strfmt(sa.name(), "=", sa.value());
}

static inline std::string
emit_attribute(const ggraph::attribute & attribute)
{
	static std::unordered_map<
		std::type_index,
		std::string(*)(const ggraph::attribute&)
	> map({
	  {std::type_index(typeid(string_attribute)), emit_string_attribute}
	});

	GGRAPH_DEBUG_ASSERT(map.find(std::type_index(typeid(attribute))) != map.end());
	return map[std::type_index(typeid(attribute))](attribute);
}

static inline std::string
emit_attributes(const operation & op)
{
	std::string str;
	for (const auto & attribute : op) {
		str += emit_attribute(attribute) + "\\n";
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

	for (size_t n = 0; n < g.nnodes(); n++)
		dot.append(strfmt(emit_node(g.node(n)), "\n"));

	for (size_t n = 0; n < g.nnodes(); n++) {
		auto node = g.node(n);
		for (size_t s = 0; s < node->nsuccessors(); s++)
			dot.append(strfmt(emit_edge(node, node->successor(s)), "\n"));
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
