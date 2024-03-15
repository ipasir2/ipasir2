# This project builds the test clients and Doxygen documentation for IPASIR-2.
# The IPASIR-2 header can be used without building this.

cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/buildutils/load_solvers.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/buildutils/doxygen.cmake)


option(WITH_DOXYGEN "Add a target `doxygen` building Doxygen files for ipasir.h" OFF)

add_subdirectory(src)