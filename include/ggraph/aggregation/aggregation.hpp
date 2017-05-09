#ifndef GGRAPH_AGGREGATION_AGGREGATION_HPP
#define GGRAPH_AGGREGATION_AGGREGATION_HPP

#include <ggraph/aggregation/node.hpp>

#include <memory>

namespace ggraph {

class graph;

namespace agg {

class node;

std::unique_ptr<agg::node>
aggregate(ggraph::graph & graph);

}}

#endif
