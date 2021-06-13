VPATH=er:er/parser
outdir := debug
parserdir := er/parser
_objs := parser.o main.o
objs := $(patsubst %,$(outdir)/%,$(_objs))
CXX := g++
CXXFLAGS := -std=c++20 -I.
libs := -lfmt
flags_deps = -MMD -MP -MF $(@:.o=.d)

all: erlisp

erlisp: $(outdir) $(objs)
	$(info Linking $@ ...)
	$(CXX) $(objs) -o $@ $(libs)

$(outdir):
	mkdir -p $(outdir)

-include $(outdir)/*.d

$(parserdir)/parser.hpp: $(parserdir)/erlisp.ypp
	$(info Using bison on $< ...)
	@bison $< --defines=$(parserdir)/parser.hpp -o $(parserdir)/erlisp.cpp.re

er/parser/erlisp.cpp.re: er/parser/erlisp.ypp
	$(info Using bison on $< ...)
	@bison $< --defines=$(parserdir)/parser.hpp -o $(parserdir)/erlisp.cpp.re

$(outdir)/parser.o: $(parserdir)/erlisp.cpp.re
	$(info Using re2c on $< ...)
	@re2c $< -o $(parserdir)/erlisp.cpp
	$(info Compiling $(parserdir)/erlisp.cpp ...)
	@$(CXX) $(CXXFLAGS) $(flags_deps) -c er/parser/erlisp.cpp -o $@

$(outdir)/%.o: %.cpp er/parser/parser.hpp
	$(info Compiling $< ...)
	@$(CXX) $(CXXFLAGS) $(flags_deps) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(outdir)
	rm $(parserdir)/erlisp.cpp.re
	rm $(parserdir)/erlisp.cpp

