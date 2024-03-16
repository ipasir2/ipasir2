// Trivial client written in C, to check that ipasir2.h remains compatible with C99

#include "ipasir2.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


void check_ip2_errorcode(char const* file, int line, char const* call, ipasir2_errorcode actual, ipasir2_errorcode expected) {
    if (actual != expected) {
        if (expected == IPASIR2_E_OK) {
            fprintf(stderr, "%s:%d: %s failed with error code %d\n", file, line, call, actual);
        }
        else {
            fprintf(stderr, "%s:%d: %s returned error code %d, but expected %d\n", file, line, call, actual, expected);
        }
        fflush(stderr);
    }
}


void check_result_impl(char const* file, int line, int actual_value, int expected_value, char const* function)
{
    if (actual_value != expected_value) {
        fprintf(stderr, "%s:%d: %s produced result %d, but expected %d\n", file, line, function, actual_value, expected_value);
    }
}


#define MACRO_ARG_STRINGIFY(x) #x
#define MACRO_ARG_TO_STRING(x) MACRO_ARG_STRINGIFY(x)
#define expect_ip2_ok(x) do {check_ip2_errorcode(__FILE__, __LINE__, MACRO_ARG_TO_STRING(x), x, IPASIR2_E_OK);} while(0);
#define expect_ip2_error(x, errorcode) do {check_ip2_errorcode(__FILE__, __LINE__, MACRO_ARG_TO_STRING(x), x, errorcode);} while(0);
#define check_result(actual, expected, function) do {check_result_impl(__FILE__, __LINE__, actual, expected, function);} while(0);


int terminate_fn() {
    return 0;
}


int main(int argc, char **argv) {
    void* solver = NULL;
    expect_ip2_ok(ipasir2_init(&solver));

    if (solver == NULL) {
        return 1;
    }

    int32_t clauses[][5] = {
        {1, 2, 3},
        {-1, 2},
        {1, -2},
        {-1}
    };

    expect_ip2_ok(ipasir2_add(solver, clauses[0], 3, IPASIR2_R_NONE));
    expect_ip2_ok(ipasir2_add(solver, clauses[1], 2, IPASIR2_R_NONE));
    expect_ip2_ok(ipasir2_add(solver, clauses[2], 2, IPASIR2_R_NONE));
    expect_ip2_ok(ipasir2_set_terminate(solver, solver, terminate_fn));

    int result = 0;
    expect_ip2_ok(ipasir2_solve(solver, &result, NULL, 0));
    check_result(result, 10, "ipasir2_solve");

    expect_ip2_ok(ipasir2_val(solver, 3, &result));
    check_result(result, 1, "ipasir2_val");

    expect_ip2_error(ipasir2_val(solver, 0, &result), IPASIR2_E_INVALID_ARGUMENT);
    expect_ip2_error(ipasir2_failed(solver, 3, &result), IPASIR2_E_INVALID_STATE);

    expect_ip2_ok(ipasir2_add(solver, clauses[3], 1, IPASIR2_R_NONE));

    int32_t assumptions[] = {-3};
    expect_ip2_ok(ipasir2_solve(solver, &result, assumptions, 1));
    check_result(result, 20, "ipasir2_solve");

    expect_ip2_error(ipasir2_val(solver, 3, &result), IPASIR2_E_INVALID_STATE);
    expect_ip2_ok(ipasir2_failed(solver, 3, &result));
    check_result(result, 1, "ipasir2_failed");

    return 0;
}
