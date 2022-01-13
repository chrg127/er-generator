CXXFLAGS := -Wall -Wextra -g -std=c++20
CXX := g++
LDLIBS := -lfmt
flags_deps = -MMD -MP -MF $(@:.o=.d)

_objs := main.cpp lexer.cpp graph.cpp parser.cpp
outdir := debug
objs := $(patsubst %,$(outdir)/%.o,$(_objs))
programname := erlisp

all: $(outdir)/$(programname)

$(outdir)/$(programname): $(outdir) $(objs) $(objs_main)
	$(info Linking $@ ...)
	@$(CXX) $(objs) -o $@ $(LDLIBS)

-include $(outdir)/*.d

$(outdir)/%.cpp.o: %.cpp
	$(info Compiling $< ...)
	@$(CXX) $(CXXFLAGS) $(flags_deps) -c $< -o $@

$(outdir):
	mkdir -p $(outdir)

.PHONY: clean

clean:
	rm -rf $(outdir)
