include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(catch STATIC catch.hh catch.cc)

link_libraries(bad catch)

add_executable(t_sequences t_sequences.cc)
target_precompile_headers(t_sequences REUSE_FROM bad)

add_executable(t_storage t_storage.cc)
target_precompile_headers(t_storage REUSE_FROM bad)

add_executable(t_tapes t_tapes.cc)
target_precompile_headers(t_tapes REUSE_FROM bad)

add_executable(t_mixed t_mixed.cc)
target_precompile_headers(t_mixed REUSE_FROM bad)

add_executable(t_disjoint t_disjoint.cc)
target_precompile_headers(t_disjoint REUSE_FROM bad)
