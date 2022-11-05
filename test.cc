#include <stdio.h>

#include "ipasir.h"

int main() {
    void* cadical = ipasir_init();
    ipasir_add(cadical, 1);
    ipasir_add(cadical, 0);
    ipasir_add(cadical, -1);
    ipasir_add(cadical, 0);
    int ret = ipasir_solve(cadical);
    if (ret == 10) {
        printf("SAT\n");
    } else if (ret == 20) {
        printf("UNSAT\n");
    } else {
        printf("UNKNOWN\n");
    }
    ipasir_release(cadical);
    return 0;
}