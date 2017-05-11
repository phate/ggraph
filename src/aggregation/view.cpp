#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/util/strfmt.hpp>

#include <typeindex>
#include <unordered_map>

namespace ggraph {
namespace agg {

/* ascii */

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

/* graphml */

class graphml_context final {
public:
	inline
	graphml_context()
	: nesting_(0)
	{}

	inline void
	push_nesting() noexcept
	{
		nesting_ += 1;
	}

	inline void
	pop_nesting() noexcept
	{
		GGRAPH_DEBUG_ASSERT(nesting_ != 0);
		nesting_ -= 1;
	}

	inline const std::string &
	last_id() const noexcept
	{
		return last_id_;
	}

	inline void
	set_last_id(const std::string & id) noexcept
	{
		last_id_ = id;
	}

	inline void
	push_ancestor(const node * ancestor) noexcept
	{
		ancestors_.push_back(ancestor);
	}

	inline void
	pop_ancestor() noexcept
	{
		GGRAPH_DEBUG_ASSERT(!ancestors_.empty());
		ancestors_.pop_back();
	}

	inline std::string
	node_id(const node * n) noexcept
	{
		if (ids_.find(n) != ids_.end())
			return ids_[n];

		std::string id;
		if (!ancestors_.empty()) {
			GGRAPH_DEBUG_ASSERT(ids_.find(ancestors_.back()) != ids_.end());
			id = ids_[ancestors_.back()] + "::";
		}

		ids_[n] = id + "n" + strfmt(ids_.size());
		return ids_[n];
	}

	inline std::string
	graph_id(const node * n) noexcept
	{
		return node_id(n) + ":";
	}

	inline std::string
	group_id(const node * n) noexcept
	{
		GGRAPH_DEBUG_ASSERT(is_forkjoin_node(n));
		return "g::" + node_id(n);
	}

	inline std::string
	fork_id(const node * n) noexcept
	{
		GGRAPH_DEBUG_ASSERT(is_forkjoin_node(n));
		return "f::" + node_id(n);
	}

	inline std::string
	join_id(const node * n) noexcept
	{
		GGRAPH_DEBUG_ASSERT(is_forkjoin_node(n));
		return "j::" + node_id(n);
	}

	inline std::string
	indent() const noexcept
	{
		return std::string(nesting_, '\t');
	}

private:
	size_t nesting_;
	std::string last_id_;
	std::vector<const node*> ancestors_;
	std::unordered_map<const node*, std::string> ids_;
};

static inline std::string
graphml_header()
{
	return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
         "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
         "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns "
         "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
}

static inline std::string
graphml_footer()
{
	return "</graphml>\n";
}

static inline std::string
node_tag(const std::string & id, graphml_context & ctx)
{
	return ctx.indent() + "<node id=\"" + id + "\"/>\n";
}

static inline std::string
node_tag(const node * n, graphml_context & ctx)
{
	return node_tag(ctx.node_id(n), ctx);
}

static inline std::string
node_starttag(const node * n, graphml_context & ctx)
{
	return ctx.indent() + "<node id=\"" + ctx.node_id(n) + "\">\n";
}

static inline std::string
node_endtag(const graphml_context & ctx)
{
	return ctx.indent() + "</node>\n";
}

static inline std::string
graph_starttag(const std::string & id, const graphml_context & ctx)
{
	return ctx.indent() + "<graph id=\"" + id + "\" edgedefault=\"undirected\">\n";
}

static inline std::string
graph_starttag(const node * n, graphml_context & ctx)
{
	return graph_starttag(ctx.graph_id(n), ctx);
}

static inline std::string
graph_endtag(const graphml_context & ctx)
{
	return ctx.indent() + "</graph>\n";
}

static inline std::string
group_starttag(const node * n, graphml_context & ctx)
{
	return ctx.indent() + "<node id=\"g::" + ctx.group_id(n) + "\">\n" +
				 ctx.indent() + "<graph id=\"" + ctx.graph_id(n) + "\" edgedefault=\"undirected\">\n";
}

static inline std::string
group_endtag(graphml_context & ctx)
{
	return graph_endtag(ctx) + node_endtag(ctx);
}

static inline std::string
join_tag(const node * n, graphml_context & ctx)
{
	return node_tag(ctx.join_id(n), ctx);
}

static inline std::string
fork_tag(const node * n, graphml_context & ctx)
{
	return node_tag(ctx.fork_id(n), ctx);
}

static inline std::string
edge_tag(const std::string & s, const std::string & t, graphml_context & ctx)
{
	if (s.empty())
		return "";

	return ctx.indent() + "<edge source=\"" + s + "\" target=\"" + t + "\"/>\n";
}

static std::string
visit_node(const node*, graphml_context&);

static inline std::string
visit_grain_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_grain_node(n));

	std::string subgraph;
	subgraph += node_tag(n, ctx);
	subgraph += edge_tag(ctx.last_id(), ctx.node_id(n), ctx);

	ctx.set_last_id(ctx.node_id(n));
	return subgraph;
}

static inline std::string
visit_forkjoin_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_forkjoin_node(n));
	std::string subgraph = group_starttag(n, ctx);

	ctx.push_nesting();
	subgraph += fork_tag(n, ctx);
	subgraph += edge_tag(ctx.last_id(), ctx.fork_id(n), ctx);

	ctx.push_ancestor(n);
	for (size_t c = 0; c < n->nchildren(); c++) {
		ctx.set_last_id(ctx.fork_id(n));
		subgraph += visit_node(n->child(c), ctx);
		subgraph += edge_tag(ctx.last_id(), ctx.join_id(n), ctx);
	}
	ctx.pop_ancestor();

	subgraph += join_tag(n, ctx);
	ctx.pop_nesting();
	ctx.set_last_id(ctx.join_id(n));

	subgraph += group_endtag(ctx);
	return subgraph;
}

static inline std::string
visit_linear_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_linear_node(n));

	std::string subgraph;
	for (size_t c = 0; c < n->nchildren(); c++)
		subgraph += visit_node(n->child(c), ctx);

	return subgraph;
}

static inline std::string
visit_node(
	const node * n,
	graphml_context & ctx)
{
	static std::unordered_map<std::type_index, std::function<std::string(
		const node*,
		graphml_context & ctx
	)>> map({
	  {std::type_index(typeid(ggraph::agg::grain_node)), visit_grain_node}
	,	{std::type_index(typeid(ggraph::agg::forkjoin_node)), visit_forkjoin_node}
	, {std::type_index(typeid(ggraph::agg::linear_node)), visit_linear_node}
	});

	auto it = map.find(std::type_index(typeid(*n)));
	GGRAPH_DEBUG_ASSERT(it != map.end());
	return it->second(n, ctx);
}

std::string
to_graphml(const node & n)
{
	graphml_context ctx;

	std::string graph = graphml_header();
	graph += graph_starttag("G", ctx);

	ctx.push_nesting();
	graph += visit_node(&n, ctx);
	ctx.pop_nesting();

	graph += graph_endtag(ctx);
	graph += graphml_footer();

	return graph;
}

/* view */

static inline void
view(const std::string s, FILE * out)
{
	fputs(s.c_str(), out);
	fflush(out);
}

void
view_str(const node & n, FILE * out)
{
	view(to_str(n), out);
}

void
view_graphml(const node & n, FILE * out)
{
	view(to_graphml(n), out);
}

}}
