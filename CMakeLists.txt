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

# add_executable(b b.cc bad.h tape.h shape.h abi.cc abi.h)
# target_link_libraries(b ${CMAKE_DL_LIBS})
# set_target_properties(b PROPERTIES ENABLE_EXPORTS 1)

