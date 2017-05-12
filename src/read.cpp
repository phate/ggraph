#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/join.hpp>
#include <ggraph/read.hpp>

#include <igraph/igraph.h>

#include <unordered_map>

static inline ggraph::node *
create_grain(const igraph_t * igraph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	ggraph::grain grain;
	return ggraph.add_node(grain, {});
}

static inline ggraph::node *
create_fork(const igraph_t * graph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	ggraph::fork fork;
	return ggraph.add_node(fork, {});
}

static inline ggraph::node *
create_join(const igraph_t * graph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	ggraph::join join;
	return ggraph.add_node(join, {});
}

static inline ggraph::node *
get_entry(const igraph_t * graph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	return ggraph.entry();
}

static inline ggraph::node *
get_exit(const igraph_t * graph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	return ggraph.exit();
}

static inline ggraph::node *
create_node(const igraph_t * igraph, igraph_integer_t vid, ggraph::graph & ggraph)
{
	static std::unordered_map<
		std::string,
		std::function<ggraph::node*(const igraph_t*, igraph_integer_t, ggraph::graph&)>
	> map({
		{"start", get_entry}
	, {"end", get_exit}
	, {"task", create_grain}
	, {"fork", create_fork}
	, {"join", create_join}
	});

	GGRAPH_DEBUG_ASSERT(igraph_cattribute_has_attr(igraph, IGRAPH_ATTRIBUTE_VERTEX, "type"));
	std::string type(igraph_cattribute_VAS(igraph, "type", vid));

	auto it = map.find(type);
	GGRAPH_DEBUG_ASSERT(it != map.end());

	return it->second(igraph, vid, ggraph);
}

namespace ggraph {

std::unique_ptr<graph>
read_graphml(FILE * in)
{
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	igraph_t igraph;
	auto r = igraph_read_graph_graphml(&igraph, in, 0);
	if (r == IGRAPH_PARSEERROR || r == IGRAPH_UNIMPLEMENTED)
		throw std::runtime_error("Unable to parse input.");

	auto ggraph = std::make_unique<graph>();

	/* create nodes */
	std::unordered_map<igraph_integer_t, node*> node_map;
	for (igraph_integer_t n = 0; n < igraph_vcount(&igraph); n++) {
		ggraph::node * node = create_node(&igraph, n, *ggraph);
		node_map[n] = node;
	}

	/* create edges */
	for (igraph_integer_t n = 0; n < igraph_ecount(&igraph); n++) {
		GGRAPH_DEBUG_ASSERT(node_map.find(IGRAPH_FROM(&igraph, n)) != node_map.end());
		GGRAPH_DEBUG_ASSERT(node_map.find(IGRAPH_TO(&igraph, n)) != node_map.end());
		node_map[IGRAPH_FROM(&igraph, n)]->add_successor(node_map[IGRAPH_TO(&igraph, n)]);
	}

	igraph_destroy(&igraph);

	return ggraph;
}

}
