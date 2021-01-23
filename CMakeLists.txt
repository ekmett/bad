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
add_compile_options($<${CXX_Clang}:-fno-strict-aliasing>)

option(DECLSPEC "support __declspec(noalias)" on)
if(DECLSPEC)
  add_compile_options($<${CXX_Clang}:-fdeclspec>)
  add_compile_definitions($<${CXX_Clang}:BAD_USE_DECLSPEC>)
endif(DECLSPEC)

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

# tape
add_executable(t_tape t_tape.cc tape.hh seq.hh catch.hh)
target_precompile_headers(t_tape REUSE_FROM t_seq)

# new storage model
add_executable(t_store t_store.cc store.hh store_expr.hh seq.hh catch.hh)
target_precompile_headers(t_store REUSE_FROM t_seq)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endif(DOXYGEN_FOUND)
if (DOXYGEN_DOT_FOUND)
  set(HAVE_DOT YES)
else()
  set(HAVE_DOT NO)
endif()
if(NOT DOT_PATH)
  get_filename_component(DOT_PATH ${DOXYGEN_DOT_EXECUTABLE} PATH)
endif()
