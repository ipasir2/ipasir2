/**
 * MIT License
 * 
 * Author: Markus Iser
 * 
 */

#ifndef IPASIR2_UTIL_H
#define IPASIR2_UTIL_H

#include <stdio.h>
#include <iostream>

#include "ipasir2.h"

#define RESULT_UNKNOWN 0
#define RESULT_SAT 10
#define RESULT_UNSAT 20

typedef std::initializer_list<int32_t> clause;
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
    return IPASIR2_E_OK;
}

std::string ipasir2_errorcode_to_string(ipasir2_errorcode err) {
    switch (err) {
        case IPASIR2_E_OK:
            return "IPASIR_E_OK";
        case IPASIR2_E_UNSUPPORTED:
            return "IPASIR_E_UNSUPPORTED";
        case IPASIR2_E_UNSUPPORTED_ARGUMENT:
            return "IPASIR_E_UNSUPPORTED_ARGUMENT";
        case IPASIR2_E_UNKNOWN:
            return "IPASIR_E_UNKNOWN";
        case IPASIR2_E_INVALID_STATE:
            return "IPASIR_E_INVALID_STATE";
        case IPASIR2_E_OPTION_UNKNOWN:
            return "IPASIR_E_OPTION_UNKNOWN";
        case IPASIR2_E_OPTION_INVALID_VALUE:
            return "IPASIR_E_OPTION_INVALID_VALUE";
        case IPASIR2_E_UNSUPPORTED_PLEDGE_LEVEL:
            return "IPASIR2_E_UNSUPPORTED_PLEDGE_LEVEL";
        default:
            return "UNKNOWN ERROR CODE";
    }
}

std::string ipasir2_state_to_string(ipasir2_state state) {
    switch (state) {
        case IPASIR2_S_CONFIG:
            return "IPASIR2_STATE_CONFIG";
        case IPASIR2_S_INPUT:
            return "IPASIR2_STATE_INPUT";
        case IPASIR2_S_SAT:
            return "IPASIR2_STATE_SAT";
        case IPASIR2_S_UNSAT:
            return "IPASIR2_STATE_UNSAT";
        case IPASIR2_S_SOLVING:
            return "IPASIR2_STATE_SOLVING";
        default:
            return "UNKNOWN STATE";
    }
}

std::ostream& operator<<(std::ostream& os, const ipasir2_errorcode& err) {
    os << ipasir2_errorcode_to_string(err);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ipasir2_option& option) {
    os << option.name << ": ";
    os << "(" << option.min << "," << option.max << ") ";
    os << ipasir2_state_to_string(option.max_state);
    if (option.tunable) {
        os << " tunable";
    }
    return os;
}

#endif // IPASIR2_UTIL_H