/**
 * @file ipasir2ipasir.h
 * @author Markus Iser (markus.iser@kit.edu)
 * @brief Wrap IPASIR solver into IPASIR 2 solver
 * @version 0.1
 * @date 2022-11-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ipasir.h"
#include "ipasir2.h"


/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Return IPASIR Configuration Options
 * 
 * The array contains all available options for the solver.
 * The array is owned by the solver and must not be freed by the caller.
 * Options in the namespace "ipasir." are reserved by IPASIR specification.
 * 
 * TODO: specification of IPASIR 2 options and namespaces
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 * 
 * @return pointer to NULL-terminated array of pointers to ipasir2_option objects.
 */
IPASIR_API ipasir2_errorcode ipasir2_options(void* S, ipasir2_option const* result) {
    return IPASIR_E_UNSUPPORTED;
}

/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Set given IPASIR Configuration Option
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* S, char const* name, void const* value) {
    return IPASIR_E_UNSUPPORTED;
}


/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Asynchronous Import of Learned Clauses
 * 
 * Set a callback which the internal solver may call while inside ipasir2_solve for importing redundant clauses (like “consume” in Lingeling). 
 * The application has the responsibility to appropriately buffer redundant clauses until the solver decides to import (some of) them via the defined callback. 
 * 
 * Effect of Callback:
 *  - literals* points to the next learned clause (zero-terminated like in ipasir2_set_learn and ipasir2_add)
 *  - literals* points to nullptr if there is no clause to consume
 *  - meta-data* points to the glue value (or sth. else?) of the returned clause (0 < glue <= size); sth. like quality or weight
 *  - Both data* and meta-data* pointers must be valid until the callback is called again or the solver returns from solve
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import_redundant_clause(void* solver,
  void (*callback)(void* solver, int** literals, void* meta_data), void* state) {
    return IPASIR_E_UNSUPPORTED;
}


/// Further Suggested IPASIR 2 Methods
/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Set callback to listen to clause deletions
 */
IPASIR_API ipasir2_errorcode ipasir2_set_delete(void* solver, void* data, void (*callback)(void* data, int32_t* clause)) {
    return IPASIR_E_UNSUPPORTED;
}


/**************************************************************************/
/************************** IPASIR 1 Land begins **************************/
/**************************************************************************/

/**
 * @brief Return the name and the version of the incremental SAT solver library.
 * 
 * New in IPASIR 2.0: Return error code, moved result to parameter
 * 
 * @return const char* Library name and version
 */
IPASIR_API ipasir2_errorcode ipasir2_signature(char const** result) {
    *result = ipasir_signature();
    return IPASIR_E_OK;
}

/**
 * @brief Construct a new solver instance and return a pointer to it.
 * 
 * New in IPASIR 2.0: Return error code, moved result to parameter
 * 
 * Use the returned pointer as the first parameter in each of the following functions.
 *
 * @return void* SAT solver
 *
 * Required state: undefined
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_init(void** result) {
    *result = ipasir_init();
    return IPASIR_E_OK;
}

/**
 * @brief Release the given solver (destructor). 
 * 
 * New in IPASIR 2.0: Return error code
 * 
 * Release all solver resources and allocated memory. 
 * The solver pointer cannot be used for any purposes after this call.
 * 
 * @param solver SAT solver
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: undefined
 */
IPASIR_API ipasir2_errorcode ipasir2_release(void* solver) {
    ipasir_release(solver);
    return IPASIR_E_OK;
}

/**
 * @brief Add the given literal into the currently added clause or finalize the clause with a 0. 
 * 
 * New in IPASIR 2.0: Return error code
 * 
 * Clauses added this way cannot be removed. 
 * The addition of removable clauses can be simulated using activation literals and assumptions.
 * Literals are encoded as (non-zero) integers as in the DIMACS formats.
 * 
 * @param solver SAT solver
 * @param lit_or_zero Literal or 0
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_add(void* solver, int32_t lit_or_zero) {
    ipasir_add(solver, lit_or_zero);
    return IPASIR_E_OK;
}

/**
 * @brief Add an assumption for the next SAT search. 
 * 
 * New in IPASIR 2.0: Return error code
 * 
 * The assumption will be used in the next call of ipasir2_solve(). 
 * After calling ipasir2_solve() all the previously added assumptions are cleared.
 * 
 * @param solver SAT solver
 * @param lit Assumption Literal
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_assume(void* solver, int32_t lit) {
    ipasir_assume(solver, lit);
    return IPASIR_E_OK;
}

/**
 * @brief Solve the formula with specified clauses under the specified assumptions.
 * 
 * New in IPASIR 2.0: Return error code, moved result to last parameter
 * 
 * If the formula is satisfiable the function returns 10 
 * and the state of the solver is changed to SAT. 
 * If the formula is unsatisfiable the function returns 20 
 * and the state of the solver is changed to UNSAT. 
 * If the search is interrupted the function returns 0 
 * and the state of the solver is changed to INPUT (cf. ipasir2_set_terminate()). 
 * This function can be called in any defined state of the solver. 
 * Note that the state of the solver _during_ execution of 'ipasir2_solve' is undefined.
 * 
 * @param solver SAT solver
 * @return int 10, 20 or 0
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_solve(void* solver, int* result) {
    *result = ipasir_solve(solver);
    return IPASIR_E_OK;
}

/**
 * @brief Return the truth value of the given literal in the found satisfying assignment.
 * 
 * New in IPASIR 2.0: Return error code, moved result to last parameter
 * 
 * Return 'lit' if True, '-lit' if False; 'ipasir2_val(lit)'
 * may return '0' if the found assignment is satisfying for both
 * valuations of lit. 
 * 
 * Each solution that agrees with all non-zero values of 
 * ipasir2_val() is a model of the formula.
 *
 * This function can only be used if ipasir2_solve() has returned 10
 * and no 'ipasir2_add' nor 'ipasir2_assume' has been called
 * since then, i.e., the state of the solver is SAT.
 * 
 * @param solver SAT solver
 * @param lit Literal
 * @return int32_t Truth value of the given literal
 *
 * Required state: SAT
 * State after: SAT
 */
IPASIR_API ipasir2_errorcode ipasir2_val(void* solver, int32_t lit, int32_t* result) {
    *result = ipasir_val(solver, lit);
    return IPASIR_E_OK;
}

/**
 * @brief Check if the given assumption literal was used to prove the
 * unsatisfiability of the formula under the assumptions
 * used for the last SAT search. Return 1 if so, 0 otherwise.
 * 
 * New in IPASIR 2.0: Return error code, moved result to last parameter
 * 
 * The formula remains unsatisfiable even just under assumption literals
 * for which ipasir2_failed() returns 1.  Note that for literals 'lit'
 * which are not assumption literals, the behavior of
 * 'ipasir2_failed(lit)' is not specified.
 *
 * This function can only be used if ipasir2_solve has returned 20 and
 * no ipasir2_add or ipasir2_assume has been called since then, i.e.,
 * the state of the solver is UNSAT.
 * 
 * @param solver 
 * @param lit 
 * @return int
 * 
 * Required state: UNSAT
 * State after: UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_failed(void* solver, int32_t lit, int* result) {
    *result = ipasir_failed(solver, lit);
    return IPASIR_E_OK;
}

/**
 * @brief Set a callback function used to indicate a termination requirement to the solver.
 * 
 * New in IPASIR 2.0: Return error code
 * 
 * The solver will periodically call this function and
 * check its return value during the search.  The ipasir2_set_terminate
 * function can be called in any state of the solver, the state remains
 * unchanged after the call.  The callback function is of the form
 * "int terminate(void * data)"
 *   - it returns a non-zero value if the solver should terminate.
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the ipasir2_set_terminate function (2nd parameter).
 * 
 * @param solver 
 * @param data 
 * @param terminate 
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, int (*terminate)(void* data)) {
    ipasir_set_terminate(solver, data, terminate);
    return IPASIR_E_OK;
}

/**
 * @brief Set a callback function for extracting learned clauses from the solver. 
 * 
 * New in IPASIR 2.0: Removed the max_length parameter.
 *  
 * The solver will call this function for each learned clause. 
 * 
 * The ipasir2_set_learn function can be called in any state of the solver, 
 * the state remains unchanged after the call. 
 * 
 * The callback function is of the form "void learn(void* data, int* clause)"
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the 2nd parameter of the ipasir2_set_learn() function.
 *   - the argument "clause" is a pointer to a null terminated integer array 
 *     containing the learned clause. 
 *   - the solver can change the memory where "clause" points to 
 *     after the function call.
 *   - the solver calls the callback function from the same thread
 *     in which ipasir2_solve() has been called.
 * 
 * Subsequent calls to ipasir2_set_learn() override the previously set callback function. 
 * Setting the callback function to NULL disables the callback.
 * 
 * @param solver SAT solver
 * @param data 
 * @param learn 
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_learn(void* solver, void* data, void (*callback)(void* data, int32_t* clause)) {
    ipasir_set_learn(solver, data, INT32_MAX, callback);
    return IPASIR_E_OK;
}

