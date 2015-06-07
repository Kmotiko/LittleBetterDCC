set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 2.8)
PROJECT(DummyCCompiler CXX)

enable_testing()

find_package(Boost 1.55.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER clang++-36)
endif()

include_directories("${PROJECT_SOURCE_DIR}")

add_definitions(-std=c++1y -Wall)

add_subdirectory(lib)
add_subdirectory(test)
