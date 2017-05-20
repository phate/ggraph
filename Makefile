CPPFLAGS = -Iinclude -DGGRAPH_DEBUG
CXXFLAGS = -Wall -Werror -g --std=c++14

LIBGGRAPH_SRC = \
	src/aggregation/aggregation.cpp \
	src/aggregation/type.cpp \
	src/aggregation/view.cpp \
	src/fork.cpp \
	src/grain.cpp \
	src/graph.cpp \
	src/join.cpp \
	src/node.cpp \
	src/operation.cpp \
	src/read.cpp \
	src/view.cpp \

all: check

libggraph.a: $(patsubst %.cpp, %.la, $(LIBGGRAPH_SRC))

include tests/Makefile.sub

%.la: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.a:
	rm -f $@
	ar clqv $@ $^
	ranlib $@

clean:
	rm -f check.log
	rm -rf tests/test-runner
	find . -name *.o -o -name *.la -o -name *.a | xargs rm -f
