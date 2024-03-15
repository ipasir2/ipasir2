include(ExternalProject)

function(load_cadical)
    set(CADICAL_DIR ${CMAKE_CURRENT_BINARY_DIR}/solvers/src/cadical_external/build)
    set(CADICAL_LIB ${CADICAL_DIR}/libcadical.a)
    message(DEBUG "Cadical Library: ${CADICAL_LIB}")
    if (EXISTS ${CADICAL_LIB})
        message(DEBUG "Cadical Found")
        add_library(cadical STATIC IMPORTED)
        set_target_properties(cadical PROPERTIES IMPORTED_LOCATION "${CADICAL_LIB}")
    else()
        message(DEBUG "Injecting External Project for Fetching and Building Cadical")
        ExternalProject_Add(cadical_external
            GIT_REPOSITORY https://github.com/ipasir2/ipasir2_solver_cadical
            GIT_TAG master
            PREFIX solvers
            CONFIGURE_COMMAND ./configure #-fsanitize=address -g
            BUILD_COMMAND make -j3
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS "${CADICAL_LIB}"
            BUILD_IN_SOURCE 1
        )
        add_library(cadical STATIC IMPORTED)
        add_dependencies(cadical cadical_external)
        set_target_properties(cadical PROPERTIES IMPORTED_LOCATION "${CADICAL_LIB}")
    endif()
endfunction()

function(load_minisat)
    set(MINISAT_DIR ${CMAKE_CURRENT_BINARY_DIR}/solvers/src/minisat_external)
    set(MINISAT_LIB "${MINISAT_DIR}-build/libminisat.a")
    message(DEBUG "Minisat Library: ${MINISAT_LIB}")
    if (EXISTS ${MINISAT_LIB})
        message(DEBUG "Minisat Found")
        add_library(minisat STATIC IMPORTED)
        set_target_properties(minisat PROPERTIES IMPORTED_LOCATION "${MINISAT_LIB}")
    else()
        message(DEBUG "Injecting External Project for Fetching and Building Minisat")
        ExternalProject_Add(minisat_external
            GIT_REPOSITORY https://github.com/ipasir2/ipasir2_solver_minisat
            GIT_TAG master
            PREFIX solvers
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS "${MINISAT_LIB}"
        )
        add_library(minisat STATIC IMPORTED)
        add_dependencies(minisat minisat_external)
        set_target_properties(minisat PROPERTIES IMPORTED_LOCATION "${MINISAT_LIB}")
    endif()
endfunction()

function(load_cms)
    set(CRYPTOMINISAT_DIR ${CMAKE_CURRENT_BINARY_DIR}/solvers/src/cms_external)
    set(CRYPTOMINISAT_LIB "${CRYPTOMINISAT_DIR}-build/lib/libipasircryptominisat5${CMAKE_SHARED_LIBRARY_SUFFIX}")
    message(DEBUG "CryptoMinisat Library: ${CRYPTOMINISAT_LIB}")
    if (EXISTS ${CRYPTOMINISAT_LIB})
        message(DEBUG "CryptoMinisat Found")
        add_library(cms SHARED IMPORTED)
        set_target_properties(cms PROPERTIES IMPORTED_LOCATION "${CRYPTOMINISAT_LIB}")
    else()
        message(DEBUG "Injecting External Project for Fetching and Building CryptoMinisat")
        ExternalProject_Add(cms_external
            GIT_REPOSITORY https://github.com/ipasir2/ipasir2_solver_cryptominisat
            GIT_TAG master
            PREFIX solvers
            INSTALL_COMMAND ""
            CMAKE_ARGS "-DIPASIR=1"
            BUILD_BYPRODUCTS "${CRYPTOMINISAT_LIB}"
        )
        add_library(cms SHARED IMPORTED)
        add_dependencies(cms cms_external)
        set_target_properties(cms PROPERTIES IMPORTED_LOCATION "${CRYPTOMINISAT_LIB}")
    endif()
endfunction()
