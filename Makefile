# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.hh)
tests := $(basename $(wildcard t_*.cc))
cmake := $(wildcard cmake/*)

open ?= open

test: build
	@ninja -C build -j 10 all
	@for i in $(tests); do \
		echo build/$$i; \
		time build/$$i; \
	done

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

# `make t_seq` will run the test
$(tests): %: build/%
	time build/$@

# `make build/t_seq` will compile it
$(addprefix build/, $(tests)): %: build
	@ninja -C build -j 10 $(notdir $@)

clean:
	@rm -rf build

.PHONY: clean run doc test
