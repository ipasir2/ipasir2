cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2)

add_executable(test test.cc)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test cadical)
target_link_libraries(test cadical)

add_executable(genipabones genipa/genipabones.cpp ipasir.h)
target_include_directories(genipabones PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipabones cadical)
target_link_libraries(genipabones cadical)

add_subdirectory(genipa/pblib)
add_executable(genipamax genipa/genipamax.cpp ipasir.h genipa/pblib/pb2cnf.h)
target_include_directories(genipamax PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipamax cadical)
target_link_libraries(genipamax cadical pblib)

add_executable(genipaessentials genipa/genipaessentials.cpp ipasir2ipasir.cc ipasir.h)
target_include_directories(genipaessentials PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipaessentials cadical)
target_link_libraries(genipaessentials cadical)

add_executable(testoptions tests/print_all_options.cc ipasir2cadical.cc ipasir2.h ipasir.h)
target_include_directories(testoptions PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(testoptions PRIVATE ${PROJECT_BINARY_DIR}/solvers/src/cadical_external/src)
add_dependencies(testoptions cadical)
target_link_libraries(testoptions cadical)
