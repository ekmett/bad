# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.hh)
cmake := $(wildcard cmake/*)
open ?= open

run: build/t_ad
	@build/t_ad

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

t_ad: build/t_ad
t_shape: build/t_shape
.PHONY: t_ad t_shape

build/t_ad: build
	@ninja -C build t_ad

build/t_shape: build
	@ninja -C build t_shape

clean:
	@rm -rf build

.PHONY: clean run doc
