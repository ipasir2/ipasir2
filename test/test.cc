#include <stdio.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ipasir2.h"

#define RESULT_UNKNOWN 0
#define RESULT_SAT 10
#define RESULT_UNSAT 20

typedef std::initializer_list<int> clause;
typedef std::initializer_list<clause> cnf;

ipasir2_errorcode ipasir2_add_clause(void* solver, clause c) {
    ipasir2_errorcode ret;
    for (auto l : c) {
        ret = ipasir2_add(solver, l);
        if (ret) {
            return ret;
        }
    }
    return ipasir2_add(solver, 0);
}

ipasir2_errorcode ipasir2_add_formula(void* solver, cnf c) {
    ipasir2_errorcode ret;
    for (auto cl : c) {
        ret = ipasir2_add_clause(solver, cl);
        if (ret) {
            return ret;
        }
    }
    return IPASIR_E_OK;
}

TEST_CASE("Trivial SAT / UNSAT") {
    ipasir2_errorcode ret;

    void* solver;
    ret = ipasir2_init(&solver);
    CHECK(ret == IPASIR_E_OK);

    SUBCASE("SAT Empty Formula") {
        int result;
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR_E_OK);
        CHECK(result == RESULT_SAT);
    }

    SUBCASE("SAT Single Variable") {
        int result;
        ret = ipasir2_add_clause(solver, { 1 });
        CHECK(ret == IPASIR_E_OK);
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR_E_OK);
        CHECK(result == RESULT_SAT);
    }

    SUBCASE("UNSAT Empty Clause") {
        int result;
        ret = ipasir2_add_clause(solver, {});
        CHECK(ret == IPASIR_E_OK);
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR_E_OK);
        CHECK(result == RESULT_UNSAT);
    }

    SUBCASE("UNSAT Single Variable") {
        int result;
        ret = ipasir2_add_formula(solver, {{ 1 }, { -1 }});
        CHECK(ret == IPASIR_E_OK);
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR_E_OK);
        CHECK(result == RESULT_UNSAT);
    }

    ret = ipasir2_release(solver);
    CHECK(ret == IPASIR_E_OK);
}

// int main() {
//     return 0;
// }