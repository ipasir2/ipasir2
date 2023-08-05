#include <stdio.h>
#include <iostream>

#include "ipasir2.h"
#include "util/ipasir2_util.h"


const std::string red("\033[0;31m");
const std::string green("\033[0;32m");
const std::string blue("\033[0;34m");
const std::string reset("\033[0m");


void print_options(void* solver) {
    ipasir2_option const* option;
    ipasir2_errorcode err = ipasir2_options(solver, &option);
    if (err) {
        std::cout << "ipasir2_options() returned " << err << std::endl;
        return;
    }
    while (option->name != nullptr) {
        std::cout << *option;
        ++option;
        if (option->name != nullptr) {
            std::cout << "; ";
        }
    }
    std::cout << std::endl;
}

void print_available(std::string function, ipasir2_errorcode err) {
    switch (err) {
        case IPASIR_E_OK:
            std::cout << green << "[available] " << reset << function << " (IPASIR_E_OK)" << std::endl;
            break;
        case IPASIR_E_UNSUPPORTED:
            std::cout << blue << "[unsupported] " << reset << function << " (IPASIR_E_UNSUPPORTED)" << std::endl;
            break;
        default:
            std::cout << red << "[error] " << reset << function << " (" << err << ")" << std::endl;
    }
}

ipasir2_errorcode probe_availabilty_of_basic_functionality(void* solver) {
    ipasir2_errorcode err = IPASIR_E_OK;
    int result;
    int32_t value;

    err = ipasir2_add_clause(solver, { 1 });
    print_available("ipasir2_add()", err);

    if (err) return err;

    err = ipasir2_solve(solver, &result);
    print_available("ipasir2_solve()", err);

    if (err) return err;

    err = ipasir2_val(solver, 1, &value);
    print_available("ipasir2_val()", err);

    if (err) return err;

    err = ipasir2_assume(solver, -1);
    print_available("ipasir2_assume()", err);

    if (err) return err;

    err = ipasir2_solve(solver, &result);

    if (err) {
        print_available("ipasir2_solve()", err);
        return err;
    }

    err = ipasir2_failed(solver, 1, &result);    
    print_available("ipasir2_failed()", err);

    return err;
}

void probe_availability_of_callbacks(void* solver) {
    ipasir2_errorcode err = IPASIR_E_OK;
    void* data;

    err = ipasir2_set_terminate(solver, data, [](void* data) {
            return 0;
        });

    print_available("ipasir2_set_terminate()", err);

    err = ipasir2_set_learn(solver, data, [](void* data, int32_t* clause) {
            std::cout << "learned a clause" << std::endl;
        });

    print_available("ipasir2_set_learn()", err);

    err = ipasir2_set_import_redundant_clause(solver, data, [](void* data) -> int32_t const* {
            std::cout << "imported a redundant clause" << std::endl;
	    return nullptr;
        });

    print_available("ipasir2_set_import_redundant_clause()", err);
}

ipasir2_errorcode probe_availability_of_options(void* solver) {
    ipasir2_errorcode err = IPASIR_E_OK;
    ipasir2_option const* option;

    err = ipasir2_options(solver, &option);
    print_available("ipasir2_options()", err);

    if (err) return err;

    if (option->name != nullptr) {
        err = ipasir2_set_option(solver, option->name, option->min);
        print_available("ipasir2_set_option()", err);
        return err;
    }
    else {
        std::cout << blue << "[unavailable] " << reset << "no actual options to set" << std::endl;
        return IPASIR_E_UNSUPPORTED;
    }
}

int main() {
    void* solver;
    ipasir2_errorcode err;
    char const* solver_name;

    err = ipasir2_signature(&solver_name);
    if (err) {
        std::cout << red << "[critical] " << reset << "ipasir2_signature() returned " << err << std::endl;
        return 1;
    }

    std::cout << "Inspecting IPASIR-2 Solver: " << solver_name << std::endl;
    
    err = ipasir2_init(&solver);
    if (err) {
        std::cout << red << "[critical] " << reset << "ipasir2_init() returned " << err << std::endl;
        return 1;
    }

    err = probe_availabilty_of_basic_functionality(solver);
    if (err) {
        std::cout << red << "[critical] " << reset << "basic functionality not available" << std::endl;
        return 1;
    }

    probe_availability_of_callbacks(solver);

    err = probe_availability_of_options(solver);

    if (!err) {
        print_options(solver);
    }

    err = ipasir2_release(solver);
    if (err) {
        std::cout << red << "[critical] " << reset << "ipasir2_release() returned " << err << std::endl;
        return 1;
    }
}
