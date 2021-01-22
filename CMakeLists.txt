# for now I assume I'm building on clang

cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(${CMAKE_SOURCE_DIR})

# required, i alias all over the place.
add_compile_options(-fno-strict-aliasing)

# i'm not interested in CPUs without at least these
# but for the new Rosetta maybe we can relax and drop -mavx/-mavx2?
add_compile_options(-msse4.2 -mavx -mavx2)

add_compile_options(-fdeclspec -DBAD_USE_DECLSPEC)

# for sanity
add_compile_options(-Wall -Wpedantic -Wextra -Wshadow -Wstrict-overflow "-Wno-#warnings")
# add_compile_options(-Werror) # for testing only

option(NATIVE "compile for the current architecture" on)
if(NATIVE)
  add_compile_options("-march=native")
endif(NATIVE)


# sanitizers

option(ASAN "address sanitization" OFF)
if(ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif(ASAN)

# add_compile_options(-O2)

add_library(abi STATIC abi.cc abi.hh map.hh attrib.hh)
link_libraries(abi)

add_executable(t_seq t_seq.cc seq.hh catch.hh)
target_precompile_headers(t_seq PRIVATE [["abi.hh"]] [["seq.hh"]])

add_executable(t_store t_store.cc store.hh seq.hh catch.hh)
target_precompile_headers(t_store REUSE_FROM t_seq)

add_executable(t_tape t_tape.cc tape.hh seq.hh catch.hh)
target_precompile_headers(t_tape REUSE_FROM t_seq)
