# yes, a makefile for a cmake build system, just to keep muscle memory working

src := $(wildcard *.cc) $(wildcard *.hh)
cmake := $(wildcard cmake/*)
open ?= open
main ?= t_symbol

run: $(main)

test: t_ad t_einsum t_shape t_store

build: CMakeLists.txt $(src) $(cmake)
	@cmake -Bbuild -GNinja

t_ad: build/t_ad
	build/t_ad

t_einsum: build/t_einsum
	build/t_einsum

t_shape: build/t_shape
	build/t_shape

t_symbol: build/t_symbol
	build/t_symbol

t_store: build/t_store
	build/t_store

x_tape: build/x_tape
	build/x_tape

.PHONY: t_ad t_einsum t_shape t_store t_symbol x_tape

build/t_ad: build
	@ninja -C build t_ad

build/t_einsum: build
	@ninja -C build t_einsum

build/t_shape: build
	@ninja -C build t_shape

build/t_store: build
	@ninja -C build t_store

build/t_symbol: build
	@ninja -C build t_symbol

build/x_tape: build
	@ninja -C build x_tape

clean:
	@rm -rf build

.PHONY: clean run doc test
