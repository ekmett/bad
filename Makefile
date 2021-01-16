# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.h)
cmake := $(wildcard cmake/*)
open ?= open

run: build/t
	@build/t

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

build/t: build
	@ninja -C build t

clean:
	@rm -rf build

.PHONY: clean run doc
