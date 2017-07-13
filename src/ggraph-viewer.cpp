#include <ggraph/aggregation/aggregation.hpp>
#include <ggraph/aggregation/view.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/read.hpp>

#include <iostream>

static inline void
print_usage(const char * file)
{
	std::cout << "Grain graph viewer\n";
	std::cout << "Usage:" << file << " FILE\n";
}

int
main(int argc, char * argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}

	std::string file(argv[1]);
	auto f = fopen(file.c_str(), "r");
	if (!f) {
		std::cerr << "Error: Cannot open file" << file << "\n";
		exit(1);
	}

	auto graph = ggraph::read_graphml(f);
	if (!is_valid(*graph)) {
		std::cerr << "Error: Invalid grain graph.";
		exit(1);
	}

	auto root = ggraph::agg::aggregate(*graph);
	ggraph::agg::propagate(*root);
	ggraph::agg::view_graphml(*root, stdout);

	fclose(f);

	return 0;
}
