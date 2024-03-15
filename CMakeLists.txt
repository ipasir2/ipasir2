# This project builds the example and test code for IPASIR2. The IPASIR2 header
# can be used without building this.

cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/build_utils/load_solvers.cmake)


function(add_solver_tool NAME SOLVER SOURCEFILE)
    add_executable(${NAME} ${SOURCEFILE})
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_dependencies(${NAME} ${SOLVER})
    target_link_libraries(${NAME} PRIVATE ${SOLVER})
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -pedantic)
    # target_compile_options(${NAME} PRIVATE -fsanitize=address)

    # TODO: fix warnings
    target_compile_options(${NAME} PRIVATE -Wno-unused)
endfunction()


load_cadical()
load_cms()
load_minisat()

foreach(solver IN ITEMS cadical cms minisat)
    add_solver_tool(test_${solver} ${solver} test/test.cc)
    add_solver_tool(test_notify_${solver} ${solver} test/test_notify.cc)
    add_solver_tool(inspect_${solver} ${solver} util/inspect.cc)
endforeach()
