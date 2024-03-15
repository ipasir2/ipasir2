/**
 * MIT License
 * 
 * Author: Markus Iser
 * 
 */

#include <stdio.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ipasir2.h"
#include "ipasir2_util.h"


TEST_CASE("Trivial SAT / UNSAT") {
    ipasir2_errorcode ret;

    void* solver;
    ret = ipasir2_init(&solver);
    CHECK(ret == IPASIR2_E_OK);

    SUBCASE("SAT Empty Formula") {
        int result;
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
    }

    SUBCASE("SAT Single Variable") {
        int result;
        ret = ipasir2_add_clause(solver, { 1 });
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
    }

    SUBCASE("UNSAT Empty Clause") {
        int result;
        ret = ipasir2_add_clause(solver, {});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_UNSAT);
    }

    SUBCASE("UNSAT Single Variable") {
        int result;
        ret = ipasir2_add_formula(solver, {{ 1 }, { -1 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_UNSAT);
    }

    ret = ipasir2_release(solver);
    CHECK(ret == IPASIR2_E_OK);
}