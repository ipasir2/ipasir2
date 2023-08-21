/**
 * IPASIR-2: The Re-entrant Incremental SAT Solver API (IPASIR) Version 2.0
 * 
 * This header specifies the API for incremental SAT solvers.
 * 
 */
#ifndef INTERFACE_IPASIR2_H_
#define INTERFACE_IPASIR2_H_

#include <stdint.h>
#include <string.h>

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
 * @enum ipasir2_errorcode
 * @brief IPASIR-2 error codes
 * @details The IPASIR-2 error codes are used to indicate the success or failure of a function call.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_OK
 *  @brief Success.
 *  @details The function call was successful.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_UNKNOWN
 *  @brief Unknown error.
 *  @details The function call failed for an unknown reason.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_UNSUPPORTED
 *  @brief Unsupported function.
 *  @details The function is not implemented by the solver.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_UNSUPPORTED_ARGUMENT
 *  @brief Unsupported argument.
 *  @details The function is not implemented for handling the given argument value.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_INVALID_STATE
 *  @brief Invalid state.
 *  @details The function call is not allowed in the current state of the solver.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_OPTION_UNKNOWN
 *  @brief Unknown option.
 *  @details The option is not supported by the solver.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_OPTION_INVALID_VALUE
 *  @brief Invalid option value.
 *  @details The option value is outside the allowed range.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_UNSUPPORTED_PLEDGE_LEVEL
 *  @brief Unsupported pledge level.
 *  @details The solver does not support the given pledge level.
 * 
 */
typedef enum ipasir2_errorcode {
    IPASIR2_E_OK = 0,
    IPASIR2_E_UNKNOWN = 1,
    IPASIR2_E_UNSUPPORTED,
    IPASIR2_E_UNSUPPORTED_ARGUMENT,
    IPASIR2_E_INVALID_STATE,
    IPASIR2_E_OPTION_UNKNOWN,
    IPASIR2_E_OPTION_INVALID_VALUE,
    IPASIR2_E_UNSUPPORTED_PLEDGE_LEVEL
} ipasir2_errorcode;


/**
 * @enum ipasir2_state
 * @brief States of the IPASIR-2 underlying state machine.
 * @details The state of the IPASIR-2 solver is defined by the state of the underlying state machine.
 *     State transitions are triggered by ipasir function calls. 
 *     The state machine is initialized in the CONFIG state.
 *     Functions are only allowed to be called in the states specified in this documentation.
 *     If a function is called in the wrong state, the function returns IPASIR2_E_INVALID_STATE.
 * 
 *     States are ordered by the following partial order: 
 *         CONFIG < INPUT = SAT = UNSAT < SOLVING
 *     
 *     For example, an IPASIR function which is allowed in states <= INPUT is 
 *     also allowed in states SAT, UNSAT and CONFIG.
 *      
 */
typedef enum ipasir2_state {
    IPASIR2_S_CONFIG = 0,
    IPASIR2_S_INPUT = 1,
    IPASIR2_S_SAT,
    IPASIR2_S_UNSAT,
    IPASIR2_S_SOLVING,
} ipasir2_state;


/**
 * @enum ipasir2_redundancy
 * @brief Redundancy type for the import clause callback.
 * @details The import clause callback is used to import clauses.
 *     The import callback setter pledges the minimally expected redundancy type. 
 *     
 *     Redundancy type pledges are ordered from stronger to weaker as follows: 
 *          EQUIVALENT > EQUISATISFIABLE > FORGETTABLE > NONE.
 * 
 * The callback function returns a clause and the redundancy type that applies to that clause.
 * This redundancy type must be at least as strong as the redundancy type pledged by the callback setter.
 * For example, if the import callback setter pledges to return clauses of type EQUISATISFIABLE, 
 * the callback may also return clauses of type EQUIVALENT, but not clauses of type FORGETTABLE or NONE.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_NONE
 *  @brief Irredundant clauses.
 *  @details The clauses served by the import clause callback are not necessarily redundant 
 *      and might change the satisfiability of the formula.
 *      Irredundant clauses might introduce new variables.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_FORGETTABLE
 *  @brief Irredundant but forgettable clauses.
 *  @details The clauses served by the import clause callback are not necessarily redundant
 *      and might change the satisfiability of the formula. 
 *      However, the solver is allowed to forget these clauses.
 *      Forgettable clauses might introduce new variables.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_EQUISATISFIABLE
 *  @brief Equisatisfiable clauses.
 *  @details The clauses served by the import clause callback are satisfiability preserving.
 *      Satisfiability preserving clauses might introduce new variables.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_EQUIVALENT
 *  @brief Equivalence preserving clauses.
 *  @details The clauses served by the import clause callback are equivalence preserving.
 *      Equivalence preserving clauses do not introduce new variables.
 * 
 */
typedef enum ipasir2_redundancy {
    IPASIR2_R_NONE = 0,
    IPASIR2_R_FORGETTABLE = 1,
    IPASIR2_R_EQUISATISFIABLE,
    IPASIR2_R_EQUIVALENT,
} ipasir2_redundancy;


/**
 * @struct ipasir2_option
 * @brief IPASIR Configuration Options
 * @details Solver options are identified by a string name.
 * 
 * If a solver does not support a given option, it must return IPASIR2_E_OPTION_UNKNOWN when the option is set.
 * 
 * @var ipasir2_option::name
 *  @brief Unique option identifier.
 *  @details Option identifers can be grouped into namespaces which are separated by a dot.
 *      The IPASIR-2 specification reserves the namespace "ipasir." for options defined by the IPASIR-2 specification.
 *      If a solver provides an option from the "ipasir." namespace, its behavior must be as specified in the IPASIR-2 specification.
 * 
 * @var ipasir2_option::min
 *  @brief Minimum value.
 *  @details Minimum allowed value for the option.
 *      The solver must return IPASIR2_E_OPTION_INVALID_VALUE if the option is set to a value outside the range [min, max].
 * 
 * @var ipasir2_option::max
 *  @brief Maximum value.
 *  @details Maximum allowed value for the option.
 *      The solver must return IPASIR2_E_OPTION_INVALID_VALUE if the option is set to a value outside the range [min, max].
 * 
 * @var ipasir2_option::max_state
 *  @brief Maximal state.
 *  @details Maximal state in which the option is allowed to be set. 
 *      The solver must return IPASIR2_E_INVALID_STATE if the option is set in a state greater than max_state.
 * 
 * @var ipasir2_option::tunable
 *  @brief Tunable option.
 *  @details Specifies if the option is eligible for tuning.
 *      If the option is not tunable, automatic tuners should not attempt to set the option.
 * 
 * @var ipasir2_option::indexed
 *  @brief Indexed option.
 *  @details Specifies if the option uses a variable index.
 *      Some options can be set individually per variable, e.g., the polarity of a variable.
 *      If the option is indexed, the index parameter of ipasir2_set_option() specifies the variable index.
 *      If the option is not indexed, the index parameter of ipasir2_set_option() is ignored.
 * 
 * @var ipasir2_option::handle
 *  @brief Option handle.
 *  @details The handle is an opaque pointer for solver internal use in the setter function.
 * 
 */
typedef struct ipasir2_option {
    char const* name;
    int64_t min;
    int64_t max;
    ipasir2_state max_state;
    bool tunable;
    bool indexed;
    void const* handle;
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
 * @brief Return the handle to the option with the given name.
 * 
 * @param solver 
 * @param name 
 * @param handle output parameter
 * @return ipasir2_errorcode
 *  - IPASIR2_E_OPTION_UNKNOWN if the option is not supported by the solver
 *  - IPASIR2_E_OK otherwise
 */
IPASIR_API inline ipasir2_errorcode ipasir2_get_option_handle(void* solver, char const* name, ipasir2_option const** handle) {
    ipasir2_option const* options;
    ipasir2_options(solver, &options);
    for (; options != nullptr; options++) {
        if (strcmp(options->name, name) == 0) {
            *handle = options;
            return IPASIR2_E_OK;
        }
    }
    return IPASIR2_E_OPTION_UNKNOWN;
}


/** 
 * @brief Set value of option identified by the given handle.
 * 
 * @param solver SAT solver
 * @param handle Option handle
 * @param index Option index, if the option is not indexed, this parameter is ignored
 * otherwise, it specifies a variable index, use zero if the value should be applied to all variables
 * @param value Option value
 * @return ipasir2_errorcode:
 *  - IPASIR2_E_OPTION_INVALID_VALUE if the option value is outside the allowed range
 *  - IPASIR2_E_INVALID_STATE if the option is not allowed to be set in the current state
 *  - IPASIR2_E_OK otherwise
 * 
 * Required state: <= handle->max_state
 * State after: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* solver, ipasir2_option const* handle, int64_t value, int64_t index);


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
 * Required state: <= INPUT
 * State after: <= INPUT
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
 * Required state: <= INPUT
 * State after: <= INPUT
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
IPASIR_API ipasir2_errorcode ipasir2_set_import(void* solver, void* data, ipasir2_redundancy pledge, 
    void (*callback)(void* data, int32_t const** clause, ipasir2_redundancy* type));


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
