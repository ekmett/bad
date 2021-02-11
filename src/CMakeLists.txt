include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(bad STATIC
  attributes.hh
  common.hh
  errors.hh
  functions.hh
  macros.hh
  memory.hh
  mixed_mode.hh
  operators.hh
  sequences.hh 
  storage.hh
  storage/namespace.hh
  storage/show_values.hh
  storage/store.hh
  storage/store_expr.hh
  storage/store_expr_iterator.hh
  storage/store_iterator.hh
  tapes.hh
  types.hh

  types.cc
)
target_precompile_headers(bad PRIVATE [["sequences.hh"]])
link_libraries(bad)
