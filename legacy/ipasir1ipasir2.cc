/**
* MIT License
* 
* @file ipasir1ipasir2.cc
* @author Markus Iser(markus.iser@kit.edu)
* @brief Wrap IPASIR-2 solver in IPASIR-1 app
* @date 2023-08-03
* 
* This file is part of IPASIR-2.
* 
*/

#include "ipasir.h"
#include "ipasir2.h"


const char* ipasir_signature() {
    char const* result;
    ipasir2_signature(&result);
    return result;
}

void* ipasir_init() {
    void* result;
    ipasir2_init(&result);
    return result;
}

void ipasir_release(void* solver) {
    ipasir2_release(solver);
}

void ipasir_add(void* solver, int32_t lit_or_zero) {
    ipasir2_add(solver, lit_or_zero);
}

void ipasir_assume(void* solver, int32_t lit) {
    ipasir2_assume(solver, lit);
}

int ipasir_solve(void* solver) {
    int result;
    ipasir2_solve(solver, &result);
    return result;
}

int32_t ipasir_val(void* solver, int32_t lit) {
    int32_t val;
    ipasir2_val(solver, lit, &val);
    return val;
}

int ipasir_failed(void* solver, int32_t lit) {
    int failed;
    ipasir2_failed(solver, lit, &failed);
    return failed;
}

void ipasir_set_terminate(void* solver, void* data, int(*terminate)(void* data)) {
    ipasir2_set_terminate(solver, data, terminate);
}

void ipasir_set_learn(void* solver, void* data, int max_length, void(*learned)(void* data, int32_t* clause)) {
    ipasir2_set_learn(solver, data, learned);
}