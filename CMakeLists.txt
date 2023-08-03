cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

include(${CMAKE_CURRENT_SOURCE_DIR}/load_solvers.cmake)

load_cms()
load_cadical()
load_minisat()


add_executable(test test.cc)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test cadical)
target_link_libraries(test cadical)

add_executable(testoptions print_all_options.cc)
target_include_directories(testoptions PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(testoptions PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/src/cadical_external/src)
add_dependencies(testoptions cadical)
target_link_libraries(testoptions cadical)