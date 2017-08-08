#include <ggraph/linear.hpp>
#include <ggraph/util/strfmt.hpp>

#include <cmath>

namespace ggraph {

static inline std::unordered_set<std::unique_ptr<attribute>>
create_attributes()
{
	std::unordered_set<std::unique_ptr<attribute>> attributes;
	attributes.insert(std::make_unique<dblattribute>("width", 50));
	attributes.insert(std::make_unique<dblattribute>("height", 50));
	attributes.insert(std::make_unique<strattribute>("name", ""));
	attributes.insert(std::make_unique<strattribute>("shape", "roundrectangle"));
	attributes.insert(std::make_unique<strattribute>("type", "linear"));
	attributes.insert(std::make_unique<dblattribute>("ngrains", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("nmembers", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("work_cycles", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("exec_cycles", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("parallel_benefit", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("work_deviation", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("mem_hier_util", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("inst_par_min", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("inst_par_median", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("inst_par_max", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("sibling_scatter", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("sibling_work_balance", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("chunk_work_balance", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("problematic", std::nan("")));
	attributes.insert(std::make_unique<dblattribute>("on_crit_path", std::nan("")));
	attributes.insert(std::make_unique<ngsattribute>("group_realizer"));

	return attributes;
}

linear::~linear()
{}

linear::linear()
: operation(create_attributes())
{
	to_strattribute(*find("name")).set_value(strfmt("l", (intptr_t)this));
}

std::string
linear::debug_string() const
{
	return "linear";
}

std::unique_ptr<operation>
linear::copy() const
{
	return std::unique_ptr<linear>(new linear(*this));
}

}
