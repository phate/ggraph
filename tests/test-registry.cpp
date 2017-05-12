#include "test-registry.hpp"

#include <assert.h>
#include <memory>
#include <unordered_map>

class unit_test final{
public:
	inline
	unit_test(
		const std::vector<std::string> & f,
		int (*r)(const std::vector<std::string>&))
	: files(f)
	, run(r)
	{}

	std::vector<std::string> files;
	int (*run)(const std::vector<std::string>&);
};

static std::unordered_map<std::string, std::unique_ptr<unit_test>> test_map;

void
register_test(
	const std::string & name,
	const std::vector<std::string> & files,
	int (*test)(const std::vector<std::string>&))
{
	assert(test_map.find(name) == test_map.end());
	test_map.insert(std::make_pair(name, std::make_unique<unit_test>(files, test)));
}

int
run_test(const std::string & name)
{
	assert(test_map.find(name) != test_map.end());
	auto test = std::move(test_map[name]);
	return test->run(test->files);
}
