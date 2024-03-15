// Trivial client written in C, to check that ipasir2.h remains compatible with C99

#include "ipasir2.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


void log_ip2_errors_impl(char const* file, int line, char const* call, ipasir2_errorcode errorcode) {
    if (errorcode != IPASIR2_E_OK) {
        fprintf(stderr, "%s:%i: %s failed with error code %i\n", file, line, call, errorcode);
        fflush(stderr);
    }
}


#define MACRO_ARG_STRINGIFY(x) #x
#define MACRO_ARG_TO_STRING(x) MACRO_ARG_STRINGIFY(x)
#define log_ip2_errors(x) do {log_ip2_errors_impl(__FILE__, __LINE__, MACRO_ARG_TO_STRING(x), x);} while(0);


int main(int argc, char **argv) {
    void* solver = NULL;
    log_ip2_errors(ipasir2_init(&solver));

    if (solver == NULL) {
        return 1;
    }

    int32_t clauses[][4] = {
        {1, 2, 3},
        {-1, 2},
        {1, -2},
        {-1}
    };

    log_ip2_errors(ipasir2_add(solver, clauses[0], 3, IPASIR2_R_NONE));
    log_ip2_errors(ipasir2_add(solver, clauses[1], 2, IPASIR2_R_NONE));
    log_ip2_errors(ipasir2_add(solver, clauses[2], 2, IPASIR2_R_NONE));
    log_ip2_errors(ipasir2_add(solver, clauses[3], 1, IPASIR2_R_NONE));
    log_ip2_errors(ipasir2_set_terminate(solver, NULL, NULL));

    int result = 0;
    log_ip2_errors(ipasir2_solve(solver, &result, NULL, 0));

    printf("Result: %i (expected: 10)\n", result);

    return result == 10 ? 0 : 1;
}
