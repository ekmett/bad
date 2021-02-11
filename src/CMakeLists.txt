include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(bad STATIC
  attributes.hh
  common.hh
  errors.hh
  functions.hh
  lists.hh
  macros.hh
  memory.hh
  mixed_mode.hh
  operators.hh
  sequences.hh 
  storage_namespace.hh
  storage_show_values.hh
  storage_store.hh
  storage_store_expr.hh
  storage_store_expr_iterator.hh
  storage_store_iterator.hh
  storage.hh
  tapes.hh
  types.hh

  types.cc
)
target_precompile_headers(bad PRIVATE [["sequences.hh"]])
link_libraries(bad)
