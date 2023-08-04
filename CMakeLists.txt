cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

include(${CMAKE_CURRENT_SOURCE_DIR}/load_solvers.cmake)

load_cms()
load_cadical()
load_minisat()

set(SOLVERS
    cms
    cadical
    minisat
)

foreach(solver ${SOLVERS})
    set(tname test_${solver})
    add_executable(${tname} test/test.cc)
    target_include_directories(${tname} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_dependencies(${tname} ${solver})
    target_link_libraries(${tname} ${solver})
    
    set(tname printoptions_${solver})
    add_executable(${tname} util/print_all_options.cc)
    target_include_directories(${tname} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_dependencies(${tname} ${solver})
    target_link_libraries(${tname} ${solver})
endforeach()