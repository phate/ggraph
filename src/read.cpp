#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/join.hpp>
#include <ggraph/read.hpp>

#include <igraph/igraph.h>

#include <unordered_map>

namespace ggraph {

static inline std::unordered_set<std::unique_ptr<attribute>>
create_attributes(
	const igraph_t * igraph,
	igraph_integer_t vid,
	const igraph_strvector_t * vnames,
	const igraph_vector_t * vtypes)
{
	std::unordered_set<std::unique_ptr<attribute>> attributes;
	for (ssize_t n = 0; n < igraph_strvector_size(vnames); n++) {
		if ("type" == std::string(STR(*vnames, n)))
			continue;

		std::string name(STR(*vnames, n));
		if (VECTOR(*vtypes)[n] == IGRAPH_ATTRIBUTE_STRING) {
			std::string value(igraph_cattribute_VAS(igraph, name.c_str(), vid));
			attributes.insert(create_string_attribute(name, value));
		}
	}

	return attributes;
}

static inline node *
create_node(
	const igraph_t * igraph,
	igraph_integer_t vid,
	const igraph_strvector_t * vnames,
	const igraph_vector_t * vtypes,
	graph & ggraph)
{
	static std::unordered_map<std::string, std::function<ggraph::node*(graph&)>> map({
	  {"start", [](graph & ggraph){ return ggraph.entry(); }}
	, {"end",   [](graph & ggraph){ return ggraph.exit(); }}
	, {"task",  [](graph & ggraph){ return create_grain(ggraph, {}); }}
	, {"fork",  [](graph & ggraph){ return create_fork(ggraph, {}); }}
	, {"join",  [](graph & ggraph){ return create_join(ggraph, {}); }}
	});

	GGRAPH_DEBUG_ASSERT(igraph_cattribute_has_attr(igraph, IGRAPH_ATTRIBUTE_VERTEX, "type"));
	std::string type(igraph_cattribute_VAS(igraph, "type", vid));

	auto it = map.find(type);
	GGRAPH_DEBUG_ASSERT(it != map.end());
	auto node = it->second(ggraph);

	return node;
}

std::unique_ptr<graph>
read_graphml(FILE * in)
{
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	igraph_t igraph;
	auto r = igraph_read_graph_graphml(&igraph, in, 0);
	if (r == IGRAPH_PARSEERROR || r == IGRAPH_UNIMPLEMENTED)
		throw std::runtime_error("Unable to parse input.");

	igraph_vector_t vtypes;
	igraph_strvector_t vnames;
	igraph_vector_init(&vtypes, 0);
	igraph_strvector_init(&vnames, 0);
	igraph_cattribute_list(&igraph, nullptr, nullptr, &vnames, &vtypes, nullptr, nullptr);

	auto ggraph = std::make_unique<graph>();

	/* create nodes */
	std::unordered_map<igraph_integer_t, node*> node_map;
	for (igraph_integer_t n = 0; n < igraph_vcount(&igraph); n++) {
		ggraph::node * node = create_node(&igraph, n, &vnames, &vtypes, *ggraph);
		node_map[n] = node;
	}

	/* create edges */
	for (igraph_integer_t n = 0; n < igraph_ecount(&igraph); n++) {
		GGRAPH_DEBUG_ASSERT(node_map.find(IGRAPH_FROM(&igraph, n)) != node_map.end());
		GGRAPH_DEBUG_ASSERT(node_map.find(IGRAPH_TO(&igraph, n)) != node_map.end());
		node_map[IGRAPH_FROM(&igraph, n)]->add_successor(node_map[IGRAPH_TO(&igraph, n)]);
	}

	igraph_vector_destroy(&vtypes);
	igraph_strvector_destroy(&vnames);
	igraph_destroy(&igraph);

	return ggraph;
}

}
