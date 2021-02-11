set(BAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE bad_headers ${BAD_INCLUDE_DIR}/*.hh)

add_library(bad STATIC
  ${bad_headers}
  types.cc
)

target_include_directories(bad
  PRIVATE
    $<BUILD_INTERFACE:${BAD_INCLUDE_DIR}/bad>
    $<INSTALL_INTERFACE:include/bad>
  PUBLIC
    $<BUILD_INTERFACE:${BAD_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_precompile_headers(bad PRIVATE [["bad/sequences.hh"]])

link_libraries(bad)
