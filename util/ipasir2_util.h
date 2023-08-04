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

std::ostream& operator<<(std::ostream& os, const ipasir2_option& option) {
    os << option.name << ": " << option.type << " (" << option.min._int << ", " << option.max._int << ")";
    return os;
}

#endif // IPASIR2_UTIL_H