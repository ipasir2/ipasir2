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

#include <vector>


class EchoingNotifier {
    public:
        std::vector<int32_t> assigns;

        EchoingNotifier() {}

        static void notify_callback_wrapper(void* context, int32_t const* assigned, int32_t const* unassigned) {
            static_cast<EchoingNotifier*>(context)->notify_callback(assigned, unassigned);
        }

    private:
        void notify_callback(int32_t const* assigned, int32_t const* unassigned) {
            printf("notified\n");
            for (int i = 0; assigned[i]; i++) {
                printf("Assigned: %d\n", assigned[i]);
            }
            for (int i = 0; unassigned[i]; i++) {
                printf("Unassigned: %d\n", unassigned[i]);
            }          
        }
};

class Notifier {
    public:
        std::vector<int32_t> assigns;

        Notifier() : assigns() {}

        static void notify_callback_wrapper(void* context, int32_t const* assigned, int32_t const* unassigned) {
            static_cast<Notifier*>(context)->notify_callback(assigned, unassigned);
        }

    private:
        void notify_callback(int32_t const* assigned, int32_t const* unassigned) {
            for (int i = 0; assigned[i]; i++) {
                assigns.push_back(assigned[i]);
            }
            for (int i = 0; unassigned[i]; i++) {
                assigns.erase(std::remove(assigns.begin(), assigns.end(), unassigned[i]), assigns.end());
            }            
        }
};

TEST_CASE("Test Notify") {
    ipasir2_errorcode ret;

    void* solver;
    ret = ipasir2_init(&solver);
    CHECK(ret == IPASIR2_E_OK);

    const ipasir2_option* dlim;
    ret = ipasir2_get_option_handle(solver, "ipasir.limits.decisions", &dlim);
    CHECK(ret == IPASIR2_E_OK);

    SUBCASE("Notify units") {
        Notifier notifier;
        int result;
        ret = ipasir2_set_notify(solver, &notifier, Notifier::notify_callback_wrapper);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1 }, { 2 }, { -2, 3 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(notifier.assigns.size() == 3);
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 1) != notifier.assigns.end());
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 2) != notifier.assigns.end());
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 3) != notifier.assigns.end());
    }

    SUBCASE("Notify units under decision limit") {
        Notifier notifier;
        int result;
        ret = ipasir2_set_notify(solver, &notifier, Notifier::notify_callback_wrapper);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_set_option(solver, dlim, 0, 0);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1 }, { 2 }, { -2, 3 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(notifier.assigns.size() == 3);
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 1) != notifier.assigns.end());
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 2) != notifier.assigns.end());
        CHECK(std::find(notifier.assigns.begin(), notifier.assigns.end(), 3) != notifier.assigns.end());
    }

    SUBCASE("Do not notify pure literals under decision limit") {
        Notifier notifier;
        int result;
        ret = ipasir2_set_notify(solver, &notifier, Notifier::notify_callback_wrapper);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_set_option(solver, dlim, 0, 0);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1, 2 }, { 3, 4 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(notifier.assigns.size() == 0);
    }

    SUBCASE("Do not notify pure literals under decision limit in second call") {
        Notifier notifier;
        int result;
        ret = ipasir2_set_notify(solver, &notifier, Notifier::notify_callback_wrapper);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_set_option(solver, dlim, 0, 0);
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_add_formula(solver, {{ 1, 2 }, { 3, 4 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(notifier.assigns.size() == 0);
        ret = ipasir2_add_formula(solver, {{ -1, -2 }, { -3, -4 }});
        CHECK(ret == IPASIR2_E_OK);
        ret = ipasir2_solve(solver, &result, nullptr, 0);
        CHECK(ret == IPASIR2_E_OK);
        CHECK(result == RESULT_SAT);
        CHECK(notifier.assigns.size() == 0);
    }

    ret = ipasir2_release(solver);
    CHECK(ret == IPASIR2_E_OK);
}