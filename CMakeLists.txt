cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(t_ad t_ad.cc ad.hh abi.cc abi.hh)
target_link_libraries(t_ad ${CMAKE_DL_LIBS})
set_target_properties(t_ad PROPERTIES ENABLE_EXPORTS 1)

add_executable(t_shape t_shape.cc shape.hh catch.hh)
target_link_libraries(t_shape ${CMAKE_DL_LIBS})
set_target_properties(t_shape PROPERTIES ENABLE_EXPORTS 1)

add_executable(t_store t_store.cc store.hh shape.hh catch.hh)
target_link_libraries(t_store ${CMAKE_DL_LIBS})
set_target_properties(t_store PROPERTIES ENABLE_EXPORTS 1)

# manual tests
add_executable(x_tape x_tape.cc tape.hh shape.hh abi.cc abi.hh)
target_link_libraries(x_tape ${CMAKE_DL_LIBS})
set_target_properties(x_tape PROPERTIES ENABLE_EXPORTS 1)

