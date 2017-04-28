#ifndef GGRAPH_STRFMT_HPP
#define GGRAPH_STRFMT_HPP

#include <sstream>
#include <string>

namespace ggraph {

template<typename... Args>
static inline void
format_to_stream(std::ostream & os, Args... args);

template<typename Arg>
static inline void
format_to_stream(std::ostream & os, const Arg & arg)
{
	os << arg;
}

template<typename Arg, typename... Args>
static inline void
format_to_stream(std::ostream & os, const Arg & arg, Args... args)
{
	os << arg;
	format_to_stream(os, args...);
}

template<typename ... Args>
static inline std::string
strfmt(Args... args)
{
	std::ostringstream os;
	format_to_stream(os, args...);
	return os.str();
}

}

#endif
