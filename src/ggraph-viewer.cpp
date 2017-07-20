#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/read.hpp>

#include <iostream>

static inline void
print_usage(const std::string & exec)
{
	std::cerr << "Grain graph viewer\n";
	std::cerr << "Usage: " << exec << " [OPTIONS] FILE\n";
	std::cerr << "Options:\n";
	std::cerr << "-a\tAggregate grain graph.\n";
	std::cerr << "-s\tSegregate grain graph.\n";
}

class cmdflags final {
public:
	inline
	cmdflags()
	: aggregate(false)
	, segregate(false)
	{}

	bool aggregate;
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
			flags.aggregate = true;
			flags.segregate = true;
			continue;
		}


		if (flag == "-a") {
			flags.aggregate = true;
			continue;
		}

		std::cerr << "ERROR: Unknown command line flag.\n";
		print_usage(flags.exec);
		exit(1);
	}

	return flags;
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

	std::unique_ptr<ggraph::agg::node> root;
	if (flags.aggregate) {
		root = ggraph::agg::aggregate(*graph);
		ggraph::agg::propagate(*root);
	}

	if (flags.segregate)
		ggraph::agg::segregate(*root);

	if (flags.aggregate)
		ggraph::agg::view_graphml(*root, stdout);

	fclose(f);

	return 0;
}
