# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.h)
cmake := $(wildcard cmake/*)
open ?= open

run: build/b
	@build/b

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

build/t: build
	@ninja -C build t

build/b: build
	@ninja -C build b

clean:
	@rm -rf build

.PHONY: clean run doc
