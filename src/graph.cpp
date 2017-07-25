#include <ggraph/fork.hpp>
#include <ggraph/grain.hpp>
#include <ggraph/graph.hpp>
#include <ggraph/join.hpp>

#include <typeindex>
#include <unordered_map>

namespace ggraph {

/* entry */

entry::~entry()
{}

std::string
entry::debug_string() const
{
	return "entry";
}

std::unique_ptr<operation>
entry::copy() const
{
	return std::make_unique<entry>(*this);
}

/* exit */

exit::~exit()
{}

std::string
exit::debug_string() const
{
	return "exit";
}

std::unique_ptr<operation>
exit::copy() const
{
	return std::make_unique<exit>(*this);
}

/* validity */

static inline bool
is_valid_entry(const ggraph::node * n)
{
	GGRAPH_DEBUG_ASSERT(is_entry(n));

	if (n->npredecessors() != 0)
		return false;

	if (n->nsuccessors() != 1)
		return false;

	return true;
}

static inline bool
is_valid_exit(const ggraph::node * n)
{
	GGRAPH_DEBUG_ASSERT(is_exit(n));

	if (n->npredecessors() != 1)
		return false;

	if (n->nsuccessors() != 0)
		return false;

	return true;
}

static inline bool
is_valid_grain(const ggraph::node * n)
{
	GGRAPH_DEBUG_ASSERT(is_grain(n->operation()));

	if (n->npredecessors() != 1)
		return false;

	if (!is_fork(n->predecessor(0)->operation())
	&& !is_join(n->predecessor(0)->operation())
	&& !is_entry(n->predecessor(0)))
		return false;

	if (n->nsuccessors() != 1)
		return false;

	if (!is_join(n->successor(0)->operation())
	&& !is_fork(n->successor(0)->operation())
	&& !is_exit(n->successor(0)))
		return false;

	return true;
}

static inline bool
is_valid_fork(const ggraph::node * n)
{
	GGRAPH_DEBUG_ASSERT(is_fork(n->operation()));

	if (n->npredecessors() != 1)
		return false;

	if (!is_grain(n->predecessor(0)->operation())
	&& !is_join(n->predecessor(0)->operation())
	&& !is_entry(n->predecessor(0)))
		return false;

	if (n->nsuccessors() == 0)
		return false;

	for (size_t s = 0; s < n->nsuccessors(); s++) {
		if (!is_grain(n->successor(s)->operation())
		&& !is_join(n->successor(s)->operation()))
			return false;
	}

	return true;
}

static inline bool
is_valid_join(const ggraph::node * n)
{
	GGRAPH_DEBUG_ASSERT(is_join(n->operation()));

	if (n->npredecessors() == 0)
		return false;

	for (size_t p = 0; p < n->npredecessors(); p++) {
		if (!is_join(n->predecessor(p)->operation())
		&& !is_grain(n->predecessor(p)->operation()))
			return false;
	}

	if (n->nsuccessors() != 1)
		return false;

	if (!is_join(n->successor(0)->operation())
	&& !is_fork(n->successor(0)->operation())
	&& !is_exit(n->successor(0)))
		return false;

	return true;
}

bool
is_valid(const ggraph::graph & g)
{
	static std::unordered_map<std::type_index, std::function<bool(const ggraph::node*)>> map({
	  {std::type_index(typeid(ggraph::entry)), is_valid_entry}
	, {std::type_index(typeid(ggraph::exit)), is_valid_exit}
	, {std::type_index(typeid(ggraph::grain)), is_valid_grain}
	, {std::type_index(typeid(ggraph::fork)), is_valid_fork}
	, {std::type_index(typeid(ggraph::join)), is_valid_join}
	});

	bool is_valid = true;
	for (const auto & node : g) {
		auto it = map.find(std::type_index(typeid(node.operation())));
		GGRAPH_DEBUG_ASSERT(it != map.end());
		is_valid = it->second(&node);
		if (!is_valid) break;
	}

	return is_valid;
}

}
