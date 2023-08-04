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

std::string ipasir2_errorcode_to_string(ipasir2_errorcode err) {
    switch (err) {
        case IPASIR_E_OK:
            return "IPASIR_E_OK";
        case IPASIR_E_UNSUPPORTED:
            return "IPASIR_E_UNSUPPORTED";
        case IPASIR_E_UNKNOWN:
            return "IPASIR_E_UNKNOWN";
        case IPASIR_E_INVALID_STATE:
            return "IPASIR_E_INVALID_STATE";
        case IPASIR_E_OUT_OF_TIME:
            return "IPASIR_E_OUT_OF_TIME";
        case IPASIR_E_OUT_OF_MEM:
            return "IPASIR_E_OUT_OF_MEM";
        case IPASIR_E_OPTION_UNKNOWN:
            return "IPASIR_E_OPTION_UNKNOWN";
        case IPASIR_E_OPTION_INVALID_VALUE:
            return "IPASIR_E_OPTION_INVALID_VALUE";
        default:
            return "UNKNOWN ERROR CODE";
    }
}

std::ostream& operator<<(std::ostream& os, const ipasir2_errorcode& err) {
    os << ipasir2_errorcode_to_string(err);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ipasir2_option& option) {
    os << option.name << ": ";
    if (option.type == FLOAT) {
        os << "FLOAT(" << option.min._flt << "," << option.max._flt << ")";
    }
    else if (option.type == INT) {
        os << "INT(" << option.min._int << "," << option.max._int << ")";
    }
    return os;
}

#endif // IPASIR2_UTIL_H