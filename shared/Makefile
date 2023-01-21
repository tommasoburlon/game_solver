CXX := g++
CXXFLAGS := -O3 -Wall
CXXLIBS := -lpthread

all: argparse

argparse: src/argparse.cpp
	$(CXX) src/argparse.cpp -I./include $(CXXLIBS) $(CXXFLAGS) -c -o $(LIBPATH)
