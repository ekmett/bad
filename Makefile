# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.hh)

tests := $(basename $(wildcard t_*.cc))
execs := $(basename $(wildcard x_*.cc))
cmake := $(wildcard cmake/*)
open ?= open
main ?= t_str

run: $(main)

test: $(tests)

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

# `make t_seq` will run the test
$(tests) $(execs): %: build/%
	@build/$@

# `make build/t_seq` will compile it
$(addprefix build/, $(tests) $(execs)): %: build
	@ninja -C build $(notdir $@)

clean:
	@rm -rf build

phony:

.PHONY: clean run doc test phony
