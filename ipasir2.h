/**
 * IPASIR-2: The Re-entrant Incremental SAT Solver API (IPASIR) Version 2.0
 * 
 * This header specifies the API for incremental SAT solvers.
 * 
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
 * @brief IPASIR Error Codes
 */
typedef enum ipasir2_errorcode {
    IPASIR2_E_OK = 0,
    IPASIR2_E_UNKNOWN = 1, // to be used if no other code applies
    IPASIR2_E_UNSUPPORTED, // to be used if a function is not supported by the solver
    IPASIR2_E_UNSUPPORTED_ARGUMENT, // to be used if a particular function argument value is not supported by the solver
    IPASIR2_E_INVALID_STATE, // to be used if a function is called in a state which is not allowed by the ipasir state machine
    IPASIR2_E_OPTION_UNKNOWN, // specific to configuration interface. to be used if an option is not supported by the solver
    IPASIR2_E_OPTION_INVALID_VALUE // specific to configuration interface. to be used if an option value is not supported by the solver
} ipasir2_errorcode;

/**
 * @brief States of the IPASIR state machine
 * 
 * States are ordered by the following partial order:
 *  CONFIG < SAT/UNSAT/INPUT < SOLVING
 */
typedef enum ipasir2_state {
    IPASIR2_S_CONFIG = 0,
    IPASIR2_S_INPUT = 1,
    IPASIR2_S_SAT = 2,
    IPASIR2_S_UNSAT = 3,
    IPASIR2_S_SOLVING = 4,
} ipasir2_state;

/**
 * @brief Pledges for the import clause callback
 * 
 * The application specifies what kind of clauses are expected to be imported by the solver.
 * 
 * Let F be the formula specified by ipasir2_lit() calls and previous import clause calls.
 * Let C be the clause to be imported by the import clause callback.
 * 
 * IPASIR-2 specifies three kinds of pledges for the import clause callback:
 * 
 * - Equivalence: F |= F, C
 *      This means clauses are expected to be entailed by the formula such that F is equivalent to F and C.
 * - Equisatisfiable: F, C and F are equisatisfiable
 *      In particular, this mode allows to import blocked clauses and blocked sets
 * - None: 
 *      This means the solver must be able to deal with any kind of clause even those changing the satisfiability of the formula.
 * 
 */
typedef enum ipasir2_pledge {
    IPASIR2_P_EQIV = 0, // imported clause is equivalence preserving
    IPASIR2_P_IMPL = 1, // imported clause is satisfiability preserving
    IPASIR2_P_NONE = 2, // imported clause could be anything
} ipasir2_pledge;


/**
 * @brief IPASIR Configuration Options
 * 
 * Solver options are identified by a string name.
 * Option identifers can be grouped into namespaces which are separated by a dot.
 * The IPASIR-2 specification reserves the namespace "ipasir." for options defined by the IPASIR-2 specification.
 * If a solver provides an option from the "ipasir." namespace, its behavior must be as specified in the IPASIR-2 specification.
 * If a solver does not support a given option, it must return IPASIR2_E_OPTION_UNKNOWN when the option is set.
 * 
 */
typedef struct ipasir2_option {
    /// @brief identifier of the option
    char const* name;

    /// @brief minimum value
    int64_t min;

    /// @brief maximum value
    int64_t max;

    /// @brief maximum of states in which the option is allowed to be set
    ipasir2_state max_state;

    /// @brief specifies if the option is eligible for tuning
    bool tunable;

    /// @brief specifies if the option uses a variable index
    bool indexed;

} ipasir2_option;


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
 * Required state: <=SOLVING
 * State after: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_options(void* solver, ipasir2_option const** result);


/** 
 * @brief Set given IPASIR Configuration Option
 * 
 * @param solver SAT solver
 * @param name Option name
 * @param index Option index, if the option is not indexed, this parameter is ignored
 * otherwise, it specifies a variable index, use zero if the value should be applied to all variables
 * @param value Option value
 * @return ipasir2_errorcode:
 *  - IPASIR2_E_OPTION_UNKNOWN if the option is not supported by the solver
 *  - IPASIR2_E_INVALID_CONFIG if the option value is invalid
 *  - IPASIR2_E_INVALID_STATE if the option is not allowed to be set in the current state
 *  - IPASIR2_E_OK otherwise
 * 
 * Required state: <=ipasir2_options[name].max_state
 * State after: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* solver, char const* name, int64_t index, int64_t value);


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
 * @param callback 
 * @return ipasir2_errorcode
 *
 * Required state: <=INPUT
 * State after: <=INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, 
    int (*callback)(void* data));

/**
 * @brief Set a callback function for extracting learned clauses from the solver. 
 *  
 * The solver will call this function for each learned clause. 
 * 
 * The ipasir2_set_learn function can be called in any state of the solver, 
 * the state remains unchanged after the call. 
 * 
 * The max_length parameter specifies the maximum length of the learned clauses to be returned.
 * If this is -1 the solver can return clauses of any length.
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
 * This function supersedes the former ipasir_set_learn() function.
 * 
 * @param solver SAT solver
 * @param data 
 * @param max_length (-1 for any length)
 * @param learn 
 * @return ipasir2_errorcode
 * 
 * Required state: <=INPUT
 * State after: <=INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_export(void* solver, void* data, int max_length, 
    void (*callback)(void* data, int32_t const* clause));


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
 * adhere to the given pledge regarding the relationship to the original formula 
 * as given by ipasir2_add() calls.
 * 
 * Solvers must take care of correctness with regard to imported clauses. 
 * 
 * Note: Function is eligible to be called in any state.
 * 
 * @TODO: State transition cases.
 *
 * @param solver SAT solver
 * @param pledge guarantees about the relationship to the original formula
 * @param data State object passed to \p import
 * @param callback Callback function
 * @return ipasir2_errorcode
 *
 * Required state: <= SOLVING
 * State after: <= SOLVING
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import(void* solver, void* data, ipasir2_pledge pledge, 
    int32_t const* (*callback)(void* data));


/**
 * @brief Sets a callback to notify about changes in the current partial assignment under analysis
 * 
 * Changes are returned for all variables that have been assigned or unassigned since the last call to the callback.
 * 
 * The solver must ensure that variables in \p assigned and \p unassigned are non-intersecting.
 * This rule has implication on the minimum frequency of calls to the \p callback.
 * @TODO: we might want to have tighter restrictions on the frequency of calls
 * 
 * All assignments must be reported to the callback before the solver switches to a different state.
 * In particular, the solver must ensure that all assignments are reported before leaving SOLVING state.
 * 
 * @param solver 
 * @param data 
 * @param observed
 * @param callback
 * @return ipasir2_errorcode 
 * 
 * Required state: <= SOLVING
 * State after: <= SOLVING
 */
IPASIR_API ipasir2_errorcode ipasir2_set_notify(void* solver, void* data, 
    void (*callback)(void* data, int32_t const* assigned, int32_t const* unassigned));


#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR2_H_
