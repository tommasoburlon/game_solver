BLDDIR := build

CXX := g++
CXXFLAGS := -O3 -Wall
CXXLIBS := -lpthread

out := sudoku

all: out

out: main.cpp
	$(CXX) main.cpp $(LIBPATH) -I$(HDRPATH) $(CXXLIBS) $(CXXFLAGS) -o $(OUTDIR)/$(out)

run:
	./$(out)

clean:
	rm $(OUTDIR)/$(out)
