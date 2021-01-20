cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(t_seq t_seq.cc seq.hh catch.hh)
target_link_libraries(t_seq ${CMAKE_DL_LIBS})
set_target_properties(t_seq PROPERTIES ENABLE_EXPORTS 1)

add_executable(t_store t_store.cc store.hh seq.hh offset.hh catch.hh abi.cc abi.hh)
target_link_libraries(t_store ${CMAKE_DL_LIBS})
set_target_properties(t_store PROPERTIES ENABLE_EXPORTS 1)

add_executable(t_str t_str.cc seq.hh catch.hh map.hh abi.cc abi.hh)
target_compile_options(t_str PUBLIC -Wno-gnu-string-literal-operator-template)
target_link_libraries(t_str ${CMAKE_DL_LIBS})
set_target_properties(t_str PROPERTIES ENABLE_EXPORTS 1)

add_executable(t_map t_map.cc map.hh catch.hh)

add_executable(t_tape t_tape.cc tape.hh seq.hh abi.cc abi.hh)
target_link_libraries(t_tape ${CMAKE_DL_LIBS})
set_target_properties(t_tape PROPERTIES ENABLE_EXPORTS 1)

#add_executable(t_ad t_ad.cc ad.hh abi.cc abi.hh)
#target_link_libraries(t_ad ${CMAKE_DL_LIBS})
#set_target_properties(t_ad PROPERTIES ENABLE_EXPORTS 1)

#add_executable(t_einsum t_einsum.cc store.hh seq.hh catch.hh abi.cc abi.hh)
#target_compile_options(t_einsum PUBLIC -Wno-gnu-string-literal-operator-template)
#target_link_libraries(t_einsum ${CMAKE_DL_LIBS})
#set_target_properties(t_einsum PROPERTIES ENABLE_EXPORTS 1)

