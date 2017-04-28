#ifndef GGRAPH_VIEW_HPP
#define GGRAPH_VIEW_HPP

namespace ggraph {

class graph;

std::string
to_dot(const graph & g);

void
view(const graph & g, FILE *);

}

#endif
