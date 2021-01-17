cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(t t.cc ad.h abi.cc abi.h)
target_link_libraries(t ${CMAKE_DL_LIBS})
message(STATUS DL: ${CMAKE_DL_LIBS})
set_target_properties(t PROPERTIES ENABLE_EXPORTS 1)

add_executable(b b.cc bad.h tape.h shape.h abi.cc abi.h)
target_link_libraries(b ${CMAKE_DL_LIBS})
set_target_properties(t PROPERTIES ENABLE_EXPORTS 1)
