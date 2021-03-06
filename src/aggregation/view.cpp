#include <ggraph/aggregation/node.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/util/strfmt.hpp>

#include <typeindex>
#include <unordered_map>

namespace ggraph {
namespace agg {

/* ascii */

static inline std::string
convert_attributes(const operation & op)
{
	std::string str("[");
	for (const auto & attribute : op)
		str += strfmt(attribute.name(), "=", attribute.value_str()) + ", ";
	str += "]";

	return str;
}

std::string
to_str(const node & n)
{
	std::function<std::string(const node & n, size_t)> f = [&] (
		const node & n,
		size_t depth
	) {
		std::string subtree(depth, '-');
		subtree += n.operation().debug_string() + convert_attributes(n.operation()) + "\n";

		for (const auto & child : n)
			subtree += f(child, depth+1);

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
		GGRAPH_DEBUG_ASSERT(is_forkjoin(n->operation())
		                 || is_linear(n->operation())
		                 || is_sibling(n->operation()));
		return "g::" + node_id(n);
	}

	inline std::string
	fork_id(const node * n) noexcept
	{
		GGRAPH_DEBUG_ASSERT(is_forkjoin(n->operation()));
		return "f::" + node_id(n);
	}

	inline std::string
	join_id(const node * n) noexcept
	{
		GGRAPH_DEBUG_ASSERT(is_forkjoin(n->operation()));
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
         "<graphml xmlns:y=\"http://www.yworks.com/xml/graphml\" "
         "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
         "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns "
         "http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n";
}

static inline std::string
graphml_footer()
{
	return "</graphml>\n";
}

static inline std::string
node_starttag(const std::string & id, graphml_context & ctx)
{
	return ctx.indent() + "<node id=\"" + id + "\">\n";
}

static inline std::string
node_starttag(const node * n, graphml_context & ctx)
{
	return node_starttag(ctx.node_id(n), ctx);
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
	return ctx.indent() + "<node id=\"g::" + ctx.group_id(n) + "\" yfiles.foldertype=\"folder\">\n";
}

static inline std::string
group_endtag(graphml_context & ctx)
{
	return graph_endtag(ctx) + node_endtag(ctx);
}

static inline std::string
join_starttag(const node * n, graphml_context & ctx)
{
	return node_starttag(ctx.join_id(n), ctx);
}

static inline std::string
join_endtag(const graphml_context & ctx)
{
	return node_endtag(ctx);
}

static inline std::string
fork_starttag(const node * n, graphml_context & ctx)
{
	return node_starttag(ctx.fork_id(n), ctx);
}

static inline std::string
fork_endtag(const graphml_context & ctx)
{
	return node_endtag(ctx);
}

static inline std::string
edge_tag(const std::string & s, const std::string & t, graphml_context & ctx)
{
	if (s.empty())
		return "";

	return ctx.indent() + "<edge source=\"" + s + "\" target=\"" + t + "\"/>\n";
}

static inline std::string
data_tag(const attribute & attribute, const graphml_context & ctx)
{
	return ctx.indent()
			 + "<data key=\"v_" + attribute.name() + "\">"
			 + attribute.value_str()
			 + "</data>";
}

static inline std::string
emit_attributes(const operation & op, const graphml_context & ctx)
{
	std::string str;
	for (const auto & attribute : op)
		str += data_tag(attribute, ctx) + "\n";

	return str;
}

static std::string
visit_node(const node*, graphml_context&);

static inline std::string
visit_grain_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_grain(n->operation()));

	std::string subgraph;
	subgraph += node_starttag(n, ctx);

	ctx.push_nesting();
	subgraph += emit_attributes(n->operation(), ctx);
	ctx.pop_nesting();

	subgraph += node_endtag(ctx);
	subgraph += edge_tag(ctx.last_id(), ctx.node_id(n), ctx);

	ctx.set_last_id(ctx.node_id(n));
	return subgraph;
}

static inline std::string
visit_forkjoin_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_forkjoin(n->operation()));
	auto fjop = static_cast<const forkjoin*>(&n->operation());

	std::string subgraph = group_starttag(n, ctx);
	ctx.push_nesting();

	subgraph += emit_attributes(n->operation(), ctx);
	subgraph += graph_starttag(n, ctx);

	/* fork node */
	ctx.push_nesting();
	subgraph += fork_starttag(n, ctx);

	ctx.push_nesting();
	subgraph += emit_attributes(fjop->fork(), ctx);
	ctx.pop_nesting();

	subgraph += fork_endtag(ctx);
	subgraph += edge_tag(ctx.last_id(), ctx.fork_id(n), ctx);

	/* join node */
	subgraph += join_starttag(n, ctx);

	ctx.push_nesting();
	subgraph += emit_attributes(fjop->join(), ctx);
	ctx.pop_nesting();

	subgraph += join_endtag(ctx);

	/* children */
	ctx.push_ancestor(n);
	for (const auto & child : *n) {
		ctx.set_last_id(ctx.fork_id(n));
		subgraph += visit_node(&child, ctx);
		subgraph += edge_tag(ctx.last_id(), ctx.join_id(n), ctx);
	}
	ctx.pop_ancestor();

	ctx.pop_nesting();
	ctx.set_last_id(ctx.join_id(n));

	ctx.pop_nesting();
	subgraph += group_endtag(ctx);
	return subgraph;
}

static inline std::string
visit_sibling_node(const node * n, graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_sibling(n->operation()));

	auto subgraph = group_starttag(n, ctx);
	ctx.push_nesting();

	subgraph += emit_attributes(n->operation(), ctx);
	subgraph += graph_starttag(n, ctx);

	for (const auto & child : *n) {
		ctx.set_last_id(ctx.fork_id(n->parent()));
		subgraph += visit_node(&child, ctx);
		subgraph += edge_tag(ctx.last_id(), ctx.join_id(n->parent()), ctx);
	}

	ctx.pop_nesting();
	subgraph += group_endtag(ctx);
	return subgraph;
}

static inline std::string
visit_linear_node(
	const node * n,
	graphml_context & ctx)
{
	GGRAPH_DEBUG_ASSERT(is_linear(n->operation()));

	std::string subgraph = group_starttag(n, ctx);
	ctx.push_nesting();

	subgraph += emit_attributes(n->operation(), ctx);
	subgraph += graph_starttag(n, ctx);

	for (const auto & child : *n)
		subgraph += visit_node(&child, ctx);

	ctx.pop_nesting();
	subgraph += group_endtag(ctx);
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
	  {std::type_index(typeid(ggraph::grain)), visit_grain_node}
	, {std::type_index(typeid(ggraph::forkjoin)), visit_forkjoin_node}
	, {std::type_index(typeid(ggraph::linear)), visit_linear_node}
	, {std::type_index(typeid(ggraph::sibling)), visit_sibling_node}
	});

	auto it = map.find(std::type_index(typeid(n->operation())));
	GGRAPH_DEBUG_ASSERT(it != map.end());
	return it->second(n, ctx);
}

static inline std::string
emit_key(const ggraph::attribute & attribute, const graphml_context & ctx)
{
	static std::unordered_map<std::type_index, std::pair<std::string, std::string>> types({
	  {std::type_index(typeid(strattribute)), {"attr.type", "string"}}
	, {std::type_index(typeid(dblattribute)), {"attr.type", "double"}}
	, {std::type_index(typeid(ngsattribute)), {"yfiles.type", "nodegraphics"}}
	});

	auto t = std::type_index(typeid(attribute));
	GGRAPH_DEBUG_ASSERT(types.find(t) != types.end());
	return ctx.indent()
				 + "<key id=\"v_" + attribute.name() + "\" "
				 + "for=\"node\" "
				 + "attr.name=\"" + attribute.name() + "\" "
				 + types[t].first + "=\"" + types[t].second + "\"/>";
}

static inline std::string
graph_attributes(const node & n, const graphml_context & ctx)
{
	std::function<void(const node &, std::unordered_map<std::string, const attribute*>&)>
	collect = [&](
		const node & n,
		std::unordered_map<std::string, const attribute*> & nattributes
	){
		for (const auto & a : n.operation()) {
			if (nattributes.find(a.name()) == nattributes.end())
				nattributes[a.name()] = &a;
		}

		for (const auto & child : n)
			collect(child, nattributes);
	};


	std::unordered_map<std::string, const attribute*> nattributes;
	collect(n, nattributes);

	std::string str;
	for (const auto & pair : nattributes)
		str += emit_key(*pair.second, ctx) + "\n";

	return str;
}

std::string
to_graphml(const node & n)
{
	graphml_context ctx;

	std::string graph = graphml_header();
	graph += graph_attributes(n, ctx);
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
