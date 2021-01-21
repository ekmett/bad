cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(${CMAKE_SOURCE_DIR})
add_compile_options(-fdeclspec -Wall -Wpedantic -Wextra)

add_library(abi STATIC abi.cc abi.hh map.hh attrib.hh)
link_libraries(abi)

add_executable(t_seq t_seq.cc seq.hh catch.hh)
target_precompile_headers(t_seq PRIVATE [["abi.hh"]] [["seq.hh"]])

add_executable(t_store t_store.cc store.hh seq.hh catch.hh)
target_precompile_headers(t_store REUSE_FROM t_seq)

add_executable(t_tape t_tape.cc tape.hh seq.hh catch.hh)
target_precompile_headers(t_tape REUSE_FROM t_seq)
