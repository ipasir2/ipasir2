# This directory contains clients for testing and analyzing IPASIR-2 implementations.

function(add_solver_tool NAME SOLVER SOURCEFILE)
    add_executable(${NAME} ${SOURCEFILE})
    target_include_directories(${NAME} PRIVATE ${PROJECT_SOURCE_DIR})
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
    add_solver_tool(test_${solver} ${solver} test.cc)
    add_solver_tool(test_notify_${solver} ${solver} test_notify.cc)
    add_solver_tool(inspect_${solver} ${solver} inspect.cc)
    add_solver_tool(c_client_${solver} ${solver} c_client.c)

    # Setting the linker language is required since the solvers are written
    # in C++ and linked statically
    set_target_properties(c_client_${solver} PROPERTIES LINKER_LANGUAGE CXX)
endforeach()
