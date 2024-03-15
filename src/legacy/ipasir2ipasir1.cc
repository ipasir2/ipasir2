/**
 * MIT License
 * 
 * @author Markus Iser
 * @brief Wrap IPASIR-1 solver in IPASIR-2 app
 * @date 2022-11-04
 * 
 * This file is part of IPASIR-2.
 * 
 */

#include "ipasir.h"
#include "ipasir2.h"

ipasir2_errorcode ipasir2_options(void* S, ipasir2_option const** result) {
    return IPASIR_E_UNSUPPORTED;
}

ipasir2_errorcode ipasir2_set_option(void* S, char const* name, void const* value) {
    return IPASIR_E_UNSUPPORTED;
}

ipasir2_errorcode ipasir2_set_import_redundant_clause(void* solver, void* data, int32_t const* (*import)(void* data)) {
    return IPASIR_E_UNSUPPORTED;
}

ipasir2_errorcode ipasir2_signature(char const** result) {
    *result = ipasir_signature();
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_init(void** result) {
    *result = ipasir_init();
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_release(void* solver) {
    ipasir_release(solver);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_add(void* solver, int32_t lit_or_zero) {
    ipasir_add(solver, lit_or_zero);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_assume(void* solver, int32_t lit) {
    ipasir_assume(solver, lit);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_solve(void* solver, int* result) {
    *result = ipasir_solve(solver);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_assignment_size(void* solver, int32_t* result) {
    return IPASIR_E_UNSUPPORTED;
}

ipasir2_errorcode ipasir2_assignment(void* solver, int32_t index, int32_t* result) {
    return IPASIR_E_UNSUPPORTED;
}

ipasir2_errorcode ipasir2_val(void* solver, int32_t lit, int32_t* result) {
    *result = ipasir_val(solver, lit);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_failed(void* solver, int32_t lit, int* result) {
    *result = ipasir_failed(solver, lit);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, int (*terminate)(void* data)) {
    ipasir_set_terminate(solver, data, terminate);
    return IPASIR_E_OK;
}

ipasir2_errorcode ipasir2_set_learn(void* solver, void* data, void (*learned)(void* data, int32_t* clause)) {
    ipasir_set_learn(solver, data, INT32_MAX, learned);
    return IPASIR_E_OK;
}

