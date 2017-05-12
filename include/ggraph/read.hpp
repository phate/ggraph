#ifndef GGRAPH_READ_HPP
#define GGRAPH_READ_HPP

#include <memory>

namespace ggraph {

class graph;

std::unique_ptr<graph>
read_graphml(FILE *);

}

#endif
