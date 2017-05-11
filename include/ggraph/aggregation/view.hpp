#ifndef GGRAPH_AGGREGATION_VIEW_HPP
#define GGRAPH_AGGREGATION_VIEW_HPP

#include <string>

namespace ggraph {
namespace agg {

class node;

std::string
to_str(const node & n);

std::string
to_graphml(const node & n);

void
view_str(const node & n, FILE *);

void
view_graphml(const node & n, FILE *);

}}

#endif
