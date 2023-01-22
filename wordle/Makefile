BLDDIR := ./build
SRCDIR := ./src
HDRFLAG := -I./include -I$(HDRPATH)

CXX := g++
CXXFLAGS := -O3 -Wall
CXXLIBS := -lpthread

out := wordle

all: out

$(BLDDIR)/solver.o: $(SRCDIR)/solver.cpp
	$(CXX) $(SRCDIR)/solver.cpp $(HDRFLAG) $(CXXLIBS) $(CXXFLAGS) -c -o $(BLDDIR)/solver.o

$(BLDDIR)/constraint.o: $(SRCDIR)/constraint.cpp
	$(CXX) $(SRCDIR)/constraint.cpp $(HDRFLAG) $(CXXLIBS) $(CXXFLAGS) -c -o $(BLDDIR)/constraint.o

$(BLDDIR)/main.o: $(SRCDIR)/main.cpp
	$(CXX) $(SRCDIR)/main.cpp $(HDRFLAG) $(CXXLIBS) $(CXXFLAGS) -c -o $(BLDDIR)/main.o

out: $(BLDDIR)/main.o $(BLDDIR)/constraint.o $(BLDDIR)/solver.o
	$(CXX) $(BLDDIR)/main.o $(BLDDIR)/solver.o $(BLDDIR)/constraint.o  $(LIBPATH) $(HDRFLAG) $(CXXLIBS) $(CXXFLAGS) -o $(OUTDIR)/$(out)

run:
	./$(out)

clean:
	rm $(OUTDIR)/$(out)
	rm -r $(BLDDIR)
	mkdir $(BLDDIR)

.PHONY: clean
