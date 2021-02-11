# I hate the clutter of in directory builds.
# I hate the inconvenience of cmake out of directory builds.
#
# This is a compromise.
#
# Let's puppet cmake from out here.

src := $(wildcard src/*.cc) $(wildcard src/*.hh)
tests := $(basename $(wildcard t/t_*.cc))
cmake := $(wildcard cmake/*)

open ?= open

all: test doc

test: build
	@ninja -C build -j 10 all
	@for i in $(notdir $(tests)); do \
		echo build/$$i; \
		time build/$$i; \
	done

build: CMakeLists.txt $(cmake)
	@cmake -Bbuild -GNinja

# `make t_seq` will run the test
$(tests): %: build/%
	time build/$(notdir $@)

# `make build/t_seq` will compile it
$(addprefix build/, $(notdir $(tests))): %: build $(src)
	@ninja -C build -j 10 $(notdir $@)

doc: build/doc
	@open build/doc/html/index.html

publish: build/doc
	# nb this skips .git and .nojekyll
	@rm -rf doc/*
	@cp -aRv build/doc/html/* doc
	# assumes subdir doc check if we're in a git repo
	@if [ -d doc/.git ]; then \
		cd doc; \
		touch .nojekyll; \
		git add *; \
		git commit -a -m "doc update"; \
		git push; \
	fi
	@open https://ekmett.github.io/bad/index.html

build/doc: build
	@ninja -C build -j 10 doc

clean:
	@rm -rf build

.PHONY: clean doc test publish
