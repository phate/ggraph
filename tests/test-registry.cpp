#include "test-registry.hpp"

#include <assert.h>
#include <memory>
#include <unordered_map>

class unit_test final{
public:
	inline constexpr
	unit_test(int (*r)())
	: run(r)
	{}

	int (*run)();
};

static std::unordered_map<std::string, std::unique_ptr<unit_test>> test_map;

void
register_test(const std::string & name, int (*test)())
{
	test_map.insert(std::make_pair(name, std::make_unique<unit_test>(test)));
}

int
run_test(const std::string & name)
{
	assert(test_map.find(name) != test_map.end());
	return test_map[name]->run();
}
