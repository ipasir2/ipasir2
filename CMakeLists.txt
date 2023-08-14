cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fsanitize=address")

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

    set(tname test_notify_${solver})
    add_executable(${tname} test/test_notify.cc)
    target_include_directories(${tname} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_dependencies(${tname} ${solver})
    target_link_libraries(${tname} ${solver})
    
    set(tname inspect_${solver})
    add_executable(${tname} util/inspect.cc)
    target_include_directories(${tname} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_dependencies(${tname} ${solver})
    target_link_libraries(${tname} ${solver})
endforeach()
