cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

project(IPASIR-2-Solvers)

set(CADICAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/solvers/src/cadical_external/build)
set(CADICAL_LIB ${CADICAL_DIR}/libcadical.a)
if (EXISTS ${CADICAL_DIR})
    message(STATUS "Cadical already built")
    add_library(cadical STATIC IMPORTED)
    set_target_properties(cadical PROPERTIES IMPORTED_LOCATION "${CADICAL_LIB}")
else()
    message(STATUS "Building Cadical")
    ExternalProject_Add(cadical_external
        GIT_REPOSITORY git@github.com:arminbiere/cadical.git 
        GIT_TAG master
        PREFIX solvers
        CONFIGURE_COMMAND ./configure
        BUILD_COMMAND make -j
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS "${CADICAL_LIB}"
        BUILD_IN_SOURCE 1
    )
    add_library(cadical STATIC IMPORTED)
    add_dependencies(cadical cadical_external)
    set_target_properties(cadical PROPERTIES IMPORTED_LOCATION "${CADICAL_LIB}")
endif()


set(MINISAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/solvers/src/minisat_external)
set(MINISAT_LIB "${MINISAT_DIR}-build/libminisat.a")
if (EXISTS ${MINISAT_DIR})
    message(STATUS "Minisat already built")
    add_library(minisat STATIC IMPORTED)
    set_target_properties(minisat PROPERTIES IMPORTED_LOCATION "${MINISAT_LIB}")
else()
    message(STATUS "Building Minisat")
    ExternalProject_Add(minisat_external
        GIT_REPOSITORY git@github.com:Udopia/minisat.git 
        GIT_TAG master
        PREFIX solvers
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS "${MINISAT_LIB}"
    )
    add_library(minisat STATIC IMPORTED)
    add_dependencies(minisat minisat_external)
    set_target_properties(minisat PROPERTIES IMPORTED_LOCATION "${MINISAT_LIB}")
endif()


set(CRYPTOMINISAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/solvers/src/cms_external)
set(CRYPTOMINISAT_LIB "${CRYPTOMINISAT_DIR}-build/libcms.a")
if (EXISTS ${CRYPTOMINISAT_DIR})
    message(STATUS "CryptoMinisat already built")
    add_library(cms STATIC IMPORTED)
    set_target_properties(cms PROPERTIES IMPORTED_LOCATION ${CRYPTOMINISAT_LIB})
else()
    message(STATUS "Building CryptoMinisat")
    ExternalProject_Add(cms_external
        GIT_REPOSITORY git@github.com:msoos/cryptominisat.git
        GIT_TAG master
        PREFIX solvers
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS ${CRYPTOMINISAT_LIB}
    )
    add_library(cms STATIC IMPORTED)
    add_dependencies(cms cms_external)
    set_target_properties(cms PROPERTIES IMPORTED_LOCATION "${CRYPTOMINISAT_LIB}")
endif()


add_executable(test test.cc)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test cadical)
target_link_libraries(test cadical)

add_executable(genipabones applications/genipa/genipabones.cpp ipasir.h)
target_include_directories(genipabones PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipabones cadical)
target_link_libraries(genipabones cadical)

add_subdirectory(applications/genipa/pblib)
add_executable(genipamax applications/genipa/genipamax.cpp ipasir.h)
target_include_directories(genipamax PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipamax cadical)
target_link_libraries(genipamax cadical pblib)

add_executable(genipaessentials applications/genipa/genipaessentials.cpp ipasir2ipasir.h ipasir.h)
target_include_directories(genipaessentials PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(genipaessentials cadical)
target_link_libraries(genipaessentials cadical)