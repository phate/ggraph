#include <ggraph/aggregation/node.hpp>

#include <algorithm>
#include <limits>
#include <typeindex>

namespace ggraph {
namespace agg {

static inline void
update_grain(const ggraph::agg::node & n)
{
	GGRAPH_DEBUG_ASSERT(n.nchildren() == 0);
	/* Nothing needs to be done. */
}

static inline void
update_aggregate_node(const ggraph::agg::node & n)
{
	double dblmin = std::numeric_limits<double>::min();
	double dblmax = std::numeric_limits<double>::max();

	std::vector<double> ipm;
	double work_cycles = 0.0;
	double exec_cycles = 0.0;
	bool problematic = false;
	bool on_crit_path = false;
	double pb = dblmax;
	double wd = dblmin;
	double mem_hier_util = dblmax;
	double inst_par_min = dblmax;
	double inst_par_max = dblmin;
	double ss = dblmin;
	double swb = dblmin;
	double cwb = dblmin;
	for (const auto & child : n) {
		auto & op = child.operation();
		if (auto a = op.find("work_cycles")) work_cycles += dblvalue(*a);
		if (auto a = op.find("exec_cycles")) exec_cycles += dblvalue(*a);
		if (auto a = op.find("parallel_benefit")) pb = std::min(pb, dblvalue(*a));
		if (auto a = op.find("work_deviation")) wd = std::max(wd, dblvalue(*a));
		if (auto a = op.find("mem_hier_util")) mem_hier_util = std::min(mem_hier_util, dblvalue(*a));
		if (auto a = op.find("inst_par_min")) inst_par_min = std::min(inst_par_min, dblvalue(*a));
		if (auto a = op.find("inst_par_median")) ipm.push_back(dblvalue(*a));
		if (auto a = op.find("inst_par_max")) inst_par_max = std::max(inst_par_max, dblvalue(*a));
		if (auto a = op.find("sibling_scatter")) ss = std::max(ss, dblvalue(*a));
		if (auto a = op.find("sibling_work_balance")) swb = std::max(swb, dblvalue(*a));
		if (auto a = op.find("chunk_work_balance")) cwb = std::max(cwb, dblvalue(*a));
		if (auto a = op.find("problematic")) problematic = (dblvalue(*a) == 1.0 || problematic);
		if (auto a = op.find("on_crit_path")) on_crit_path = (dblvalue(*a) == 1.0 || on_crit_path);
	}
	std::nth_element(ipm.begin(), ipm.begin()+ipm.size()/2, ipm.end());

	auto & op = n.operation();
	if (work_cycles != 0.0) to_dblattribute(*op.find("work_cycles")).set_value(work_cycles);
	if (exec_cycles != 0.0) to_dblattribute(*op.find("exec_cycles")).set_value(exec_cycles);
	if (pb != dblmax) to_dblattribute(*op.find("parallel_benefit")).set_value(pb);
	if (wd != dblmin) to_dblattribute(*op.find("work_deviation")).set_value(wd);
	if (mem_hier_util != dblmax) to_dblattribute(*op.find("mem_hier_util")).set_value(mem_hier_util);
	if (inst_par_min != dblmax) to_dblattribute(*op.find("inst_par_min")).set_value(inst_par_min);
	if (!ipm.empty()) to_dblattribute(*op.find("inst_par_median")).set_value(ipm[ipm.size()/2]);
	if (inst_par_max != dblmin) to_dblattribute(*op.find("inst_par_max")).set_value(inst_par_max);
	if (ss != dblmin) to_dblattribute(*op.find("sibling_scatter")).set_value(ss);
	if (swb != dblmin) to_dblattribute(*op.find("sibling_work_balance")).set_value(swb);
	if (cwb != dblmin) to_dblattribute(*op.find("chunk_work_balance")).set_value(cwb);
	to_dblattribute(*op.find("problematic")).set_value(problematic ? 1.0 : 0.0);
	to_dblattribute(*op.find("on_crit_path")).set_value(on_crit_path ? 1.0 : 0.0);
}

static inline void
update_forkjoin(const ggraph::agg::node & n)
{
	update_aggregate_node(n);
}

static inline void
update_linear(const ggraph::agg::node & n)
{
	update_aggregate_node(n);
}

void
propagate(const ggraph::agg::node & n)
{
	for (const auto & child : n)
		propagate(child);

	static std::unordered_map<std::type_index, void(*)(const agg::node & n)> map({
	  {std::type_index(typeid(ggraph::grain)), update_grain}
	, {std::type_index(typeid(ggraph::forkjoin)), update_forkjoin}
	, {std::type_index(typeid(ggraph::linear)), update_linear}
	});

	GGRAPH_DEBUG_ASSERT(map.find(std::type_index(typeid(n.operation()))) != map.end());
	map[std::type_index(typeid(n.operation()))](n);
}

void
segregate(node & n)
{
	auto is_problematic = [](const node * n)
	{
		auto attribute = n->operation().find("problematic");
		if (!attribute) return false;

		return dblvalue(*attribute) ? true : false;
	};

	for (auto child = n.first_child(); child != nullptr; child = child->next_sibling())
		segregate(*child);

	if (is_forkjoin(n.operation())) {
		auto fjop = static_cast<const ggraph::forkjoin*>(&n.operation());

		/* collect all non-problematic children */
		std::vector<node*> npchildren;
		for (auto child = n.first_child(); child != nullptr; child = child->next_sibling()) {
			if (!is_problematic(child))
				npchildren.push_back(child);
		}

		/* if we have problematic children, segregate */
		if (npchildren.size() != n.nchildren()) {
			auto fjnode = create_forkjoin_node(fjop->fork(), fjop->join());
			for (const auto & child : npchildren)
				fjnode->add_child(child->detach());
			n.add_child(std::move(fjnode));
		}
	}
}

size_t
max_open_nodes(const node & n)
{
	size_t max = 0;
	for (const auto & child : n)
		max = std::max(max, max_open_nodes(child));

	if (is_grain(n.operation()))
		return 1;

	if (is_linear(n.operation()))
		return max + (n.nchildren()-1);

	if (is_forkjoin(n.operation()))
		return max + (n.nchildren()-1) + 2;

	GGRAPH_DEBUG_ASSERT(0);
	return 0;
}

void
prune(ggraph::agg::node & n)
{
	auto child = n.first_child();
	while (child) {
		auto next = child->next_sibling();
		prune(*child);
		child = next;
	}

	if (n.nchildren() == 0 && !is_grain(n.operation()))
		n.detach();
}

}}
