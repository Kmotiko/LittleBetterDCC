set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 2.8)
PROJECT(DummyCCompiler CXX)

enable_testing()

find_package(Boost 1.55.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER clang++-3.6)
endif()

if(NOT LLVM_CONFIG)
  set(LLVM_CONFIG llvm-config-3.6)
endif()

execute_process(COMMAND ${LLVM_CONFIG}  --libdir
  OUTPUT_VARIABLE LLVM_LIB_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
execute_process(COMMAND ${LLVM_CONFIG}  --includedir
  OUTPUT_VARIABLE LLVM_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
execute_process(COMMAND ${LLVM_CONFIG}  --libs
  OUTPUT_VARIABLE LLVM_LIB_FLAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

list(APPEND LLVM_LIB_NAMES
     ncurses
     pthread
     z
     m
     dl)
string(REGEX MATCHALL "-l([^ ]+)" LLVM_LIBS ${LLVM_LIB_FLAG})
foreach(var ${LLVM_LIBS})
  string(REGEX REPLACE "-l([^ ]+)" "\\1" LIB_NAME ${var})
  list(APPEND LLVM_LIB_NAMES ${LIB_NAME})
endforeach(var)

set (LLVM_CXX_FLAGS
  -D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -fvisibility-inlines-hidden -fno-common -Wcast-qual
)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${LLVM_INCLUDE_DIR}")
link_directories("${LLVM_LIB_DIR}")

add_definitions(-std=c++1y -Wall)
add_definitions(${LLVM_CXX_FLAGS})

add_subdirectory(lib)
add_subdirectory(test)
