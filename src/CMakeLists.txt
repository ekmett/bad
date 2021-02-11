include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE bad_headers ${CMAKE_SOURCE_DIR}/include/*.hh)

add_library(bad STATIC
  ${bad_headers}
  types.cc
)

target_precompile_headers(bad PRIVATE [["bad/sequences.hh"]])

link_libraries(bad)
