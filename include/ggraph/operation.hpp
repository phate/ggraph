#ifndef GGRAPH_OPERATION_HPP
#define GGRAPH_OPERATION_HPP

#include <memory>
#include <string>

namespace ggraph {

class operation {
public:
	virtual
	~operation();

	inline constexpr
	operation()
	{}

	virtual std::string
	debug_string() const = 0;

	virtual std::unique_ptr<operation>
	copy() const = 0;
};

}

#endif
