#ifndef GGRAPH_COMMON_HPP
#define GGRAPH_COMMON_HPP

#include <assert.h>

#define GGRAPH_ASSERT(x) assert(x)

#ifdef GGRAPH_DEBUG
#	define GGRAPH_DEBUG_ASSERT(x) assert(x)
#else
#	define GGRAPH_DEBUG_ASSERT(x) (void)(x)
#endif

#endif
