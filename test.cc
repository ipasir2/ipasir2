#include <stdio.h>

#include "ipasir2.h"

int main() {
    void* solver;
    ipasir2_init(&solver);
    ipasir2_add(solver, 1);
    ipasir2_add(solver, 0);
    ipasir2_add(solver, -1);
    ipasir2_add(solver, 0);
    int ret;
    ipasir2_solve(solver, &ret);
    if (ret == 10) {
        printf("SAT\n");
    } else if (ret == 20) {
        printf("UNSAT\n");
    } else {
        printf("UNKNOWN\n");
    }
    ipasir2_release(solver);
    return 0;
}