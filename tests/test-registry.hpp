#ifndef TEST_REGISTRY_HPP
#define TEST_REGISTRY_HPP

#include <string>
#include <vector>

void
register_test(
	const std::string & name,
	const std::vector<std::string>&,
	int(*test)(const std::vector<std::string>&));

int
run_test(const std::string & name);

#define GGRAPH_TEST_REGISTER(name, files, test) \
	static void __attribute__((constructor)) register_iotest(void) \
	{ \
		register_test(name, files, test); \
	}

#endif
