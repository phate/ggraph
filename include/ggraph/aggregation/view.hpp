#ifndef GGRAPH_AGGREGATION_VIEW_HPP
#define GGRAPH_AGGREGATION_VIEW_HPP

#include <string>

namespace ggraph {
namespace agg {

class node;

std::string
to_str(const node & n);

void
view(const node & n, FILE *);

}}

#endif
