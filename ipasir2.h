/**
 * IPASIR-2: The Re-entrant Incremental SAT Solver API (IPASIR) Version 2.0
 * 
 * This header file defines the API for incremental SAT solvers.
 * 
 * (c) 2022 by the IPASIR authors 
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
 * @brief IPASIR 2.0 Error Codes
 */
typedef enum ipasir2_errorcode {
    IPASIR_E_OK = 0,
    IPASIR_E_UNKNOWN = 1, // to be used if no other code applies
    IPASIR_E_UNSUPPORTED = 2,
    IPASIR_E_INVALID_STATE = 3, // to be used if a function is called in a state which is not allowed by the ipasir state machine
    IPASIR_E_OPTION_UNKNOWN = 4,
    IPASIR_E_OPTION_INVALID_VALUE = 5
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
 * To cover setting of limits:
 * - ipasir.limits.conflicts: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no conflict limit
 *    - 0: no conflicts (exit on first conflict)
 *    - n: at most n conflicts
 * - ipasir.limits.decisions: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no decision limit
 *    - 0: no decisions (only unit propagation)
 *    - n: at most n decisions
 * - ...
 * 
 * To cover phase setting:
 * - ipasir.phase.initial: set the initial phase of a variable x to true if value = x, false if value = -x
 * - ipasir.phase.initial.all: int, minimum: -1, maximum: 1, default: 0
 *   - -1: set all variables to false
 *   - 1: set all variables to true
 *   - 0: use the default phase initialization of the solver
 * 
 * To cover enabling/disabling preprocessing:
 * - ipasir.preprocessing: ...
 * 
 * 
 */

/**
 * @brief Specification of states for the IPASIR state machine
 * @TODO: weave-in new state "config" everywhere
 */
typedef enum ipasir2_state {
    IPASIR2_STATE_CONFIG = 0,
    IPASIR2_STATE_INPUT = 1, // a.k.a. UNKOWN
    IPASIR2_STATE_SAT = 2,
    IPASIR2_STATE_UNSAT = 3,
    IPASIR2_STATE_SOLVING = 4,
} ipasir2_state;

typedef struct ipasir2_option {
    /// @brief identifier of the option
    char const* name;

    /// @brief minimum value
    int64_t min;

    /// @brief maximum value
    int64_t max;

    /// @brief allowed states in which the option is allowed to be set
    /// larger ones shall entail smaller ones for the following partial order: CONFIG < SAT/UNSAT/INPUT < SOLVING 
    ipasir2_state max_state;

    /// @brief specifies if the option is eligible for tuning
    bool tunable;

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
 * Required state: CONFIG or INPUT or SAT or UNSAT
 * State after: same as before
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
 *  - IPASIR_E_INVALID_STATE if the option is not allowed to be set in the current state
 *  - IPASIR_E_OK otherwise
 * 
 * Required state: any state <= ipasir2_option.max_state
 * State after: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* solver, char const* name, int64_t value);


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
 * The callback function has the signature "void learn(void* data, int32_t const* clause)"
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
IPASIR_API ipasir2_errorcode ipasir2_set_learn(void* solver, void* data, void (*learned)(void* data, int32_t const* clause));


/**
 * @brief Sets a callback to notify about changes in the current partial assignment under analysis
 * 
 * Changes are returned for all variables that have been assigned or unassigned since the last call to the callback.
 * 
 * Assigned and backtrack are non-intersecting regarding variables (-> implication on frequency of calls)
 * 
 * @TODO: frequency of calls: at least once per decisions. relate to other api functions
 * @TODO: is_decision must have same lenght as assigned.
 * @TODO: previous decisions which are now implied ones must be notified as well (even if value did not change). 
 * @TODO: elaborete on correctness issues emerging from imported clauses
 * 
 * @TODO:
 * - decision-level can not be necessarily be determined by recording and analysing those notifications alone. 
 * - applications keeping track of decision levels can not check what is now implied at level zero (see IPASIR-UP: is_fixed)
 * 
 * @param solver 
 * @param data 
 * @param notify
 * @return ipasir2_errorcode 
 * 
 * Required state: CONFIG
 */
IPASIR_API ipasir2_errorcode ipasir2_set_notify_assignment(void* solver, void* data, 
    void (*notify)(void* data, int32_t const* assigned, int32_t const* backtrack, int8_t const* is_decision));


/**
 * @brief Sets a callback for asynchronous import of redundant clauses
 *
 * Sets a callback which may be called by the solver during ipasir2_solve()
 * for importing redundant clauses (like “consume” in Lingeling). The
 * application has the responsibility to appropriately buffer redundant clauses
 * until the solver decides to import (some of) them via the defined callback.
 *
 * The \p import callback must return a pointer to the next redundant clause to
 * import (zero-terminated like in ipasir2_set_learn()), or nullptr if there is
 * no further clause to import. If a pointer to a clause is returned, that
 * clause must be valid until \p import is called again or ipasir2_solve()
 * terminates, whichever happens first.
 * 
 * Applications using that callback must make sure that the imported clauses 
 * are entailed by the formula given by ipasir2_add() calls.
 *
 * @param solver SAT solver
 * @param import Callback function
 * @param data State object passed to \p import
 * @return ipasir2_errorcode
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import_redundant_clause(void* solver, void* data, int32_t const* (*import)(void* data));


/**
 * @brief Sets a callback for asynchronous import of redundant clauses
 *
 * Sets a callback which may be called by the solver during ipasir2_solve()
 * for importing irredundant clauses (like “consume” in Lingeling). The
 * application has the responsibility to appropriately buffer redundant clauses
 * until the solver decides to import (some of) them via the defined callback.
 *
 * The \p import callback must return a pointer to the next redundant clause to
 * import (zero-terminated like in ipasir2_set_learn()), or nullptr if there is
 * no further clause to import. If a pointer to a clause is returned, that
 * clause must be valid until \p import is called again or ipasir2_solve()
 * terminates, whichever happens first.
 * 
 * Applications must make sure that imported clauses contain only variables 
 * as indicated by \p allowed. If \p allowed is nullptr, all variables are allowed.
 * 
 * Solver provides must take care of correctness with regard to allowed variables. 
 * Either freeze or reconstruct on demand.
 * 
 * Note: Function is eligible to be called in any state.
 *
 * @param solver SAT solver
 * @param data State object passed to \p import
 * @param allowed Array of allowed variables (terminated by 0); nullptr if all variables are allowed
 * @param import Callback function
 * @return ipasir2_errorcode
 *
 * Required state: <=SOLVE
 * State after: <=SOLVE
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import_irredundant_clause(void* solver, void* data, int32_t* allowed, int32_t const* (*import)(void* data));

#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR2_H_
