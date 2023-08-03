/**
 * @file ipasir2.h
 * @author Markus Iser, Felix Kutzner, Armin Biere, Tomas Balyo, Carsten Sinz
 * @brief Re-entrant Incremental SAT Solver API (IPASIR2)
 * @version 2.0
 * @date 2022-11-02
 *  
 * @copyright Copyright (c) 2022
 */
#ifndef INTERFACE_IPASIR2_H_
#define INTERFACE_IPASIR2_H_

#include <stdint.h>

/*
 * In this header, the macro IPASIR_API is defined as follows:
 * - if IPASIR_SHARED_LIB is not defined, then IPASIR_API is defined, but empty.
 * - if IPASIR_SHARED_LIB is defined...
 *    - ...and if BUILDING_IPASIR_SHARED_LIB is not defined, IPASIR_API is
 *      defined to contain symbol visibility attributes for importing symbols
 *      of a DSO (including the __declspec rsp. __attribute__ keywords).
 *    - ...and if BUILDING_IPASIR_SHARED_LIB is defined, IPASIR_API is defined
 *      to contain symbol visibility attributes for exporting symbols from a
 *      DSO (including the __declspec rsp. __attribute__ keywords).
 */
#if defined(IPASIR_SHARED_LIB)
    #if defined(_WIN32) || defined(__CYGWIN__)
        #if defined(BUILDING_IPASIR_SHARED_LIB)
            #if defined(__GNUC__)
                #define IPASIR_API __attribute__((dllexport))
            #elif defined(_MSC_VER)
                #define IPASIR_API __declspec(dllexport)
            #endif
        #else
            #if defined(__GNUC__)
                #define IPASIR_API __attribute__((dllimport))
            #elif defined(_MSC_VER)
                #define IPASIR_API __declspec(dllimport)
            #endif
        #endif
    #elif defined(__GNUC__)
        #define IPASIR_API __attribute__((visibility("default")))
    #endif

    #if !defined(IPASIR_API)
        #if !defined(IPASIR_SUPPRESS_WARNINGS)
            #warning "Unknown compiler. Not adding visibility information to IPASIR symbols."
            #warning "Define IPASIR_SUPPRESS_WARNINGS to suppress this warning."
        #endif
        #define IPASIR_API
    #endif
#else
    #define IPASIR_API
#endif

#ifdef __cplusplus
extern "C" {
#endif



/**
 * #################################################################################################################
 * ##### IPASIR 2.0 Draft
 * #####
 * ##### The following is new in IPASIR 2.0:
 * #####
 * ##### - Configuration:    A generic configuration interface
 * ##### - Clause Sharing:   A callback interface for asynchronous import of shared clauses
 * ##### - Error Codes:      All functions now return an error code; values are returned via output parameters
 * ##### - Assignment Stack: A new interface for accessing the solver's assignment stack in INPUT and SAT state
 * #####
 * #################################################################################################################
 */


/**
 * @brief IPASIR 2.0 Error Codes
 */
typedef enum ipasir2_errorcode {
  IPASIR_E_OK = 0,
  IPASIR_E_UNKNOWN = 1, // to be used if no other code applies
  IPASIR_E_UNSUPPORTED = 2,
  IPASIR_E_INVALID_STATE = 3, // to be used if a function is called in a state which is not allowed by the ipasir state machine
  IPASIR_E_OUT_OF_TIME = 4,
  IPASIR_E_OUT_OF_MEM = 5,
  IPASIR_E_OPTION_UNKNOWN = 6,
  IPASIR_E_OPTION_INVALID_VALUE = 7
} ipasir2_errorcode;


/** 
 * @brief Specification of options for the configuration interface
 * 
 * Solver options are identified by a string name.
 * Option identifers can be grouped into namespaces which are separated by a dot.
 * The IPASIR-2 specification reserves the namespace "ipasir." for options defined by the IPASIR-2 specification.
 * If a solver provides an option from the "ipasir." namespace, its behavior must be as specified in the IPASIR-2 specification.
 * If a solver does not support a given option, it must return IPASIR_E_OPTION_UNKNOWN when the option is set.
 * 
 * The following options are defined by the IPASIR-2 specification:
 * 
 * - ipasir.preprocessing: 
 * - ipasir.preprocessing.subsumption: int, minimum: 0, maximum: 1
 * - ipasir.preprocessing.bve: int, minimum: 0, maximum: 1
 * - ipasir.preprocessing.sbva: int, minimum: 0, maximum: 1
 * - ...
 * 
 * - ipasir.limits:
 * - ipasir.limits.conflicts: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no conflict limit
 *    - 0: no conflicts (exit on first conflict)
 *    - n: at most n conflicts
 * - ipasir.limits.decisions: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no decision limit
 *    - 0: no decisions (only unit propagation)
 *    - n: at most n decisions
 * - ...
 */

typedef enum ipasir2_option_type {
    INT = 0,
    FLOAT = 1
} ipasir2_option_type;

typedef union ipasir2_option_value {
    int _int;
    float _flt;
} ipasir2_option_value;

typedef struct ipasir2_option {
    /// identifier for the option
    char const* name;

    /// @brief type identifier
    ipasir2_option_type type;

    /// @brief minimum value
    ipasir2_option_value min;

    /// @brief maximum value
    ipasir2_option_value max;
} ipasir2_option;


/** 
 * @brief Return IPASIR Configuration Options
 * 
 * The array contains all available options for the solver.
 * The array is owned by the solver and must not be freed by the caller.
 * Options in the namespace "ipasir." are reserved by IPASIR specification.
 * 
 * @param solver SAT solver
 * @param result Output parameter: pointer to NULL-terminated array of pointers to ipasir2_option objects
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 * 
 * @return pointer to NULL-terminated array of pointers to ipasir2_option objects.
 */
IPASIR_API ipasir2_errorcode ipasir2_options(void* solver, ipasir2_option const** result);

/** 
 * @brief Set given IPASIR Configuration Option
 * 
 * @param solver SAT solver
 * @param name Option name
 * @param value Option value
 * @return ipasir2_errorcode:
 *  - IPASIR_E_OPTION_UNKNOWN if the option is not supported by the solver
 *  - IPASIR_E_INVALID_CONFIG if the option value is invalid
 *  - IPASIR_E_OK otherwise
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* solver, char const* name, ipasir2_option_value value);


/**
 * @brief Asynchronous Import of Learned Clauses
 * 
 * Set a callback which the internal solver may call while inside ipasir2_solve for importing redundant clauses (like “consume” in Lingeling). 
 * The application has the responsibility to appropriately buffer redundant clauses until the solver decides to import (some of) them via the defined callback. 
 * 
 * Effect of Callback:
 *  - literals* points to the next learned clause (zero-terminated like in ipasir2_set_learn and ipasir2_add)
 *  - literals* points to nullptr if there is no clause to consume
 * 
 * @param solver SAT solver
 * @param callback Callback function
 * @param data State object passed to callback function
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import_redundant_clause(void* solver, void* data, void (*import)(void* data, int** literals));


/**
 * @brief Return the number of variables on the solver's assignment stack.
 * 
 * In INPUT state, the number of variables on the assignment stack is 
 * the number of variables in the current partial (or full) assignment, 
 * e.g., if a decision limit was reached during search.
 * 
 * In SAT state, the number of variables on the assignment stack is
 * the number of variables in the satisfying assignment.
 * 
 * @param solver SAT solver
 * @param &lit Number of variables on the assignment stack
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT
 * State after: INPUT or SAT
 */

IPASIR_API ipasir2_errorcode ipasir2_assignment_size(void* solver, int32_t* lit);

/**
 * @brief Return the assignment at the given position on the solver's assignment stack.
 * 
 * This function is intended to be used after a limited call to ipasir2_solve.
 * The provided assignment must be guaranteed to not be refuted by unit-propagation.
 * 
 * In INPUT state, the assignment at the given position on the assignment stack is
 * the assignment of the variable at the given position in the current partial assignment,
 * e.g., if a decision limit was reached during search.
 * 
 * In SAT state, the assignment at the given position on the assignment stack is
 * the assignment of the variable at the given position in the satisfying assignment.
 * The value of ipasir2_val(lit) must be equal to the assignment at the given position.
 * 
 * @param solver SAT solver
 * @param index Index of the assignment on the assignment stack
 * @param &lit Assignment at the given position on the assignment stack
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT
 * State after: INPUT or SAT
 */

IPASIR_API ipasir2_errorcode ipasir2_assignment(void* solver, int32_t index, int32_t* lit);


/**
 * @brief Return the name and the version of the incremental SAT solver library.
 * 
 * @param const char* output parameter returns library name and version
 * @return ipasir2_errorcode
 */
IPASIR_API ipasir2_errorcode ipasir2_signature(char const** result);

/**
 * @brief Construct a new solver instance and set result to return a pointer to it.
 * 
 * Use the returned pointer as the first parameter in each of the following functions.
 *
 * @param void* output parameter returns SAT solver instance
 * @return ipasir2_errorcode
 *
 * Required state: undefined
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_init(void** result);

/**
 * @brief Release the given solver (destructor). 
 * 
 * Release all solver resources and allocated memory. 
 * The solver pointer cannot be used for any purposes after this call.
 * 
 * @param solver SAT solver instance to be released
 * @return ipasir2_errorcode
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: undefined
 */
IPASIR_API ipasir2_errorcode ipasir2_release(void* solver);

/**
 * @brief Add the given literal into the currently added clause or finalize the clause with a 0. 
 * 
 * Clauses added this way cannot be removed. 
 * The addition of removable clauses can be simulated using activation literals and assumptions.
 * Literals are encoded as (non-zero) integers as in the DIMACS formats.
 * 
 * @param solver SAT solver
 * @param lit_or_zero Literal or 0
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_add(void* solver, int32_t lit_or_zero);

/**
 * @brief Add an assumption for the next SAT search. 
 * 
 * The assumption will be used in the next call of ipasir2_solve(). 
 * After calling ipasir2_solve() all the previously added assumptions are cleared.
 * 
 * @param solver SAT solver
 * @param lit Assumption Literal
 * @return ipasir2_errorcode
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_assume(void* solver, int32_t lit);

/**
 * @brief Solve the formula with specified clauses under the specified assumptions.
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
 * @param int* output parameter returns solve result 10 (SAT), 20 (UNSAT) or 0 (INDETERMINATE)
 * @return ipasir2_errorcode
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_solve(void* solver, int* result);

/**
 * @brief Return the truth value of the given literal in the found satisfying assignment.
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
 * @param int32_t* output parameter returns truth value of the given literal
 * @return ipasir2_errorcode
 *
 * Required state: SAT
 * State after: SAT
 */
IPASIR_API ipasir2_errorcode ipasir2_val(void* solver, int32_t lit, int32_t* result);

/**
 * @brief Check if the given assumption literal was used to prove the
 * unsatisfiability of the formula under the assumptions
 * used for the last SAT search. Return 1 if so, 0 otherwise.
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
 * @param int* output parameter returns 1 if the given assumption literal was used to prove unsatisfiability
 * @return ipasir2_errorcode
 * 
 * Required state: UNSAT
 * State after: UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_failed(void* solver, int32_t lit, int* result);

/**
 * @brief Set a callback function used to indicate a termination requirement to the solver.
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
 * @return ipasir2_errorcode
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, int (*terminate)(void* data));

/**
 * @brief Set a callback function for extracting learned clauses from the solver. 
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
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_learn(void* solver, void* data, void (*learned)(void* data, int32_t* clause));

#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR2_H_
