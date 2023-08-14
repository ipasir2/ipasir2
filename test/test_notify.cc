/**
 * Some tests for the ipasir2 interface.
 * 
 * @author Markus Iser 
 */

#include <stdio.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ipasir2.h"
#include "util/ipasir2_util.h"

#include <vector>


void echoing_notify_callback(void* state, int32_t const* assigned, int32_t const* unassigned) {
    printf("notified\n");
    for (int i = 0; assigned[i]; i++) {
        printf("Assigned: %d\n", assigned[i]);
    }
    for (int i = 0; unassigned[i]; i++) {
        printf("Unassigned: %d\n", unassigned[i]);
    }
}

std::vector<int32_t> assigned;
void notify_callback(void* state, int32_t const* assigned, int32_t const* unassigned) {
    for (int i = 0; assigned[i]; i++) {
        ::assigned.push_back(assigned[i]);
    }
    for (int i = 0; unassigned[i]; i++) {
        ::assigned.erase(std::remove(::assigned.begin(), ::assigned.end(), unassigned[i]), ::assigned.end());
    }
}

TEST_CASE("Test Notify") {
    ipasir2_errorcode ret;

    void* solver;
    ret = ipasir2_init(&solver);
    CHECK(ret == IPASIR2_E_OK);

    SUBCASE("Notify Unit and Unit Propagate") {
        assigned.clear();
        int result;
        ret = ipasir2_set_notify(solver, solver, notify_callback);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1 }, { 2 }, { -2, 3 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(::assigned.size() == 3);
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 1) != ::assigned.end());
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 2) != ::assigned.end());
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 3) != ::assigned.end());
    }

    SUBCASE("Notify Unit and Unit Propagate under Decision Limit") {
        assigned.clear();
        int result;
        ret = ipasir2_set_notify(solver, solver, notify_callback);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_set_option(solver, "ipasir.limits.decisions", 0, 0);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1 }, { 2 }, { -2, 3 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(::assigned.size() == 3);
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 1) != ::assigned.end());
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 2) != ::assigned.end());
        CHECK(std::find(::assigned.begin(), ::assigned.end(), 3) != ::assigned.end());
    }

    ret = ipasir2_release(solver);
    CHECK(ret == IPASIR2_E_OK);
}