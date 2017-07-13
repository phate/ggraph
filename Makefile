CPPFLAGS = -Iinclude -DGGRAPH_DEBUG
CXXFLAGS = -Wall -Werror -g --std=c++14

LIBGGRAPH_SRC = \
	src/aggregation/aggregation.cpp \
	src/aggregation/node.cpp \
	src/aggregation/view.cpp \
	src/attribute.cpp \
	src/fork.cpp \
	src/forkjoin.cpp \
	src/grain.cpp \
	src/graph.cpp \
	src/join.cpp \
	src/linear.cpp \
	src/node.cpp \
	src/operation.cpp \
	src/read.cpp \
	src/view.cpp \

GGRAPH_VIEWER_SRC = \
	src/ggraph-viewer.cpp

all: libggraph.a ggraph-viewer check

libggraph.a: $(patsubst %.cpp, %.la, $(LIBGGRAPH_SRC))

ggraph-viewer: LDFLAGS+=-L. -lggraph -ligraph
ggraph-viewer: $(patsubst %.cpp, %.o, $(GGRAPH_VIEWER_SRC)) libggraph.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

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
	rm -rf ggraph-viewer
	rm -rf tests/test-runner
	find . -name *.o -o -name *.la -o -name *.a | xargs rm -f
