include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(catch STATIC catch.hh catch.cc)
set_source_files_properties(catch.cc PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

link_libraries(bad catch)

add_executable(t_sequences t_sequences.cc)
target_precompile_headers(t_sequences PRIVATE [["bad.hh"]] [["catch.hh"]])

add_executable(t_storage t_storage.cc)
target_precompile_headers(t_storage REUSE_FROM t_sequences)

add_executable(t_tapes t_tapes.cc)
target_precompile_headers(t_tapes REUSE_FROM t_sequences)

add_executable(t_mixed t_mixed.cc)
target_precompile_headers(t_mixed REUSE_FROM t_sequences)

add_executable(t_disjoint t_disjoint.cc)
target_precompile_headers(t_disjoint REUSE_FROM t_sequences)

add_executable(t_links t_links.cc)
target_precompile_headers(t_links REUSE_FROM t_sequences)
