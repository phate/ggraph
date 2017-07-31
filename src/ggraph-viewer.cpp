#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/read.hpp>

#include <typeindex>
#include <iostream>

static inline void
print_usage(const std::string & exec)
{
	std::cerr << "Grain graph viewer\n";
	std::cerr << "Usage: " << exec << " [OPTIONS] FILE\n";
	std::cerr << "Options:\n";
	std::cerr << "-d\tPrint maximum depth of aggregation tree.\n";
	std::cerr << "-g\tPrint aggregated grain graph as graphml.\n";
	std::cerr << "-m\tPrint maximum number of open nodes.\n";
	std::cerr << "-n\tPrint number of nodes.\n";
	std::cerr << "-s\tSegregate grain graph.\n";
	std::cerr << "-t\tPrint theta of problematic grains.\n";
}

class cmdflags final {
public:
	inline
	cmdflags()
	: theta(false)
	, nnodes(false)
	, graphml(false)
	, maxdepth(false)
	, maxnodes(false)
	, segregate(false)
	{}

	bool theta;
	bool nnodes;
	bool graphml;
	bool maxdepth;
	bool maxnodes;
	bool segregate;
	std::string exec;
	std::string ggraph;
};

cmdflags
parse_cmdflags(int argc, char * argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}

	cmdflags flags;
	flags.exec= argv[0];
	flags.ggraph = argv[argc-1];
	for (int n = 1; n < argc-1; n++) {
		std::string flag(argv[n]);
		if (flag == "-s") {
			flags.segregate = true;
			continue;
		}

		if (flag == "-m") {
			flags.maxnodes = true;
			continue;
		}

		if (flag == "-n") {
			flags.nnodes = true;
			continue;
		}

		if (flag == "-g") {
			flags.graphml = true;
			continue;
		}

		if (flag == "-d") {
			flags.maxdepth = true;
			continue;
		}

		if (flag == "-t") {
			flags.theta = true;
			continue;
		}

		std::cerr << "ERROR: Unknown command line flag.\n";
		print_usage(flags.exec);
		exit(1);
	}

	return flags;
}

typedef struct nnodes {
	inline
	nnodes()
	: ngrain(0)
	, nlinear(0)
	, nsibling(0)
	, nforkjoin(0)
	{}

	size_t ngrain;
	size_t nlinear;
	size_t nsibling;
	size_t nforkjoin;
} nnodes;

static inline nnodes
count_nodes(const ggraph::agg::node & node)
{
	using namespace ggraph;

	std::function<void(const agg::node&, struct nnodes&)> count = [&](
		const agg::node & node,
		struct nnodes & nnodes
	){
		static std::unordered_map<std::type_index, void(*)(struct nnodes&)> map({
		  {std::type_index(typeid(grain)), [](struct nnodes & n){ n.ngrain++; }}
		, {std::type_index(typeid(forkjoin)), [](struct nnodes & n){ n.nforkjoin++; }}
		, {std::type_index(typeid(linear)), [](struct nnodes & n){ n.nlinear++; }}
		, {std::type_index(typeid(sibling)), [](struct nnodes & n){ n.nsibling++; }}
		});

		for (const auto & child : node)
			count(child, nnodes);

		GGRAPH_ASSERT(map.find(std::type_index(typeid(node.operation()))) != map.end());
		map[std::type_index(typeid(node.operation()))](nnodes);
	};

	struct nnodes nnodes;
	count(node, nnodes);
	return nnodes;
}

static inline size_t
max_depth(const ggraph::agg::node & node)
{
	if (node.nchildren() == 0)
		return 0;

	size_t max = 0;
	for (const auto & child : node)
		max = std::max(max, max_depth(child));

	return max+1;
}

static inline void
theta_problematic_grains(
	const ggraph::agg::node & n,
	std::unordered_map<const ggraph::agg::node*, size_t> & map)
{
	for (const auto & child : n)
		theta_problematic_grains(child, map);

	if (is_grain(n.operation()) && is_problematic(n.operation()))
		map[&n] = ggraph::agg::theta(n);
}

int
main(int argc, char * argv[])
{
	cmdflags flags = parse_cmdflags(argc, argv);

	auto f = fopen(flags.ggraph.c_str(), "r");
	if (!f) {
		std::cerr << "Error: Cannot open file" << flags.ggraph << "\n";
		exit(1);
	}

	auto graph = ggraph::read_graphml(f);
	if (!is_valid(*graph)) {
		std::cerr << "Error: Invalid grain graph.";
		exit(1);
	}

	auto root = ggraph::agg::aggregate(*graph);
	normalize(*root);
	ggraph::agg::propagate(*root);

	if (flags.segregate) {
		ggraph::agg::segregate(*root);
		ggraph::agg::propagate(*root);
	}

	if (flags.nnodes) {
		struct nnodes nnodes = count_nodes(*root);
		size_t ngroup = nnodes.nforkjoin + nnodes.nlinear + nnodes.nsibling;
		std::cout << "Grain nodes: " << nnodes.ngrain << "\n";
		std::cout << "Fork-Join nodes: " << nnodes.nforkjoin << "\n";
		std::cout << "Linear nodes: " << nnodes.nlinear << "\n";
		std::cout << "Sibling nodes: " << nnodes.nsibling << "\n";
		std::cout << "Group nodes: " << ngroup << "\n";
		std::cout << "Total nodes: " << ngroup + nnodes.ngrain << "\n";
	}

	if (flags.maxnodes)
		std::cout << max_open_nodes(*root) << "\n";

	if (flags.maxdepth)
		std::cout << max_depth(*root) << "\n";

	if (flags.theta) {
		std::unordered_map<const ggraph::agg::node*, size_t> map;
		theta_problematic_grains(*root, map);

		size_t max = 0;
		for (const auto & pair : map) {
			std::cout << strvalue(*pair.first->operation().find("name")) << ": " << pair.second << "\n";
			max = std::max(max, pair.second);
		}
		std::cout << "\n# Grains: " << map.size() << "\n";
		std::cout << "Max: " << max << "\n";
	}

	if (flags.graphml)
		ggraph::agg::view_graphml(*root, stdout);

	fclose(f);

	return 0;
}
