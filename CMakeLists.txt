# for now I assume I'm building on clang

cmake_minimum_required(VERSION 3.10)
project(bad VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(${CMAKE_SOURCE_DIR})

# to trim verbosity a bit
set(CXX_Clang "$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>")
set(Link_Clang "$<LINK_LANG_AND_ID:CXX,AppleClang,Clang>")

# I change types everywhere
# add_compile_options($<${CXX_Clang}:-fno-strict-aliasing>)
add_compile_options($<${CXX_Clang}:-fno-strict-aliasing>)

# enable support for __declspec(noalias) on clang
add_compile_options($<${CXX_Clang}:-fdeclspec>)
add_compile_definitions($<${CXX_Clang}:BAD_USE_DECLSPEC>)

# useful warnings
add_compile_options(
  $<${CXX_Clang}:-Wall>
  $<${CXX_Clang}:-Wpedantic>
  $<${CXX_Clang}:-Wextra>
  $<${CXX_Clang}:-Wshadow>
  $<${CXX_Clang}:-Wstrict-overflow>
  $<${CXX_Clang}:-Wno-\#warnings>
)

####################################################################
# * target platform
####################################################################

# just compile for the local machine for now. add options later as needed
add_compile_options($<${CXX_Clang}:-march=native>)

# TODO: disable avx on rosetta
add_compile_options(
  $<${CXX_Clang}:-msse4.2>
  $<${CXX_Clang}:-mavx>
  $<${CXX_Clang}:-mavx2>
)

# sanitizers
option(ASAN "address sanitization" off)
if(ASAN)
  add_compile_options(
    $<${CXX_Clang}:-fsanitize=address>
    $<${CXX_Clang}:-fomit-frame-pointer>
  )
  # this should also ensure the linker is clang
  add_link_options(
    $<${Link_Clang>:-fsanitize=address>
    $<${Link_Clang>:-fno-omit-frame-pointer>
  )
endif(ASAN)

# add_compile_options(-O2)

add_library(abi STATIC abi.cc abi.hh map.hh attrib.hh)
link_libraries(abi)

add_executable(t_seq t_seq.cc seq.hh catch.hh)
target_precompile_headers(t_seq PRIVATE [["abi.hh"]] [["seq.hh"]])

add_executable(t_store t_store.cc store.hh seq.hh catch.hh)
target_precompile_headers(t_store REUSE_FROM t_seq)

add_executable(t_tape t_tape.cc tape.hh seq.hh catch.hh)
target_precompile_headers(t_tape REUSE_FROM t_seq)
