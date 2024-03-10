/**
 * IPASIR-2: The Re-entrant Incremental SAT Solver API (IPASIR) Version 2.0
 * 
 * This header specifies the API for incremental SAT solvers.
 * 
 * Terminology
 * -----------
 *
 * In IPASIR2, solver instances are represented by opaque pointers.
 *
 * An IPASIR2 function is considered to be called on a solver instance S if S if F is
 * called and S is passed to the IPASIR2 function as the first argument. F is considered
 * to be executing on S if it has been called on S and has not yet finished execution.
 *
 *
 * Thread Safety (draft)
 * ---------------------
 *
 * IPASIR2 allows using multiple solvers in parallel. The intention is to cover use
 * cases where the client starts an arbitrary number of solver instances in
 * parallel, but interacts with each individual solver instance only sequentially.
 * For example, it is legal to add clauses to two different solver instances in
 * parallel, but executing ipasir2_add() in parallel on the same solver instance
 * is illegal.
 *
 * Each IPASIR2 function F belongs to one of the following categories:
 *
 *  - Type A: F has no restrictions regarding multithreading.
 *
 *  - Type B: Let S be the solver instance (ie. the first argument of the function).
 *      F may be called on S from any thread if calling F on S is legal in the current
 *      state of S, and any of the two following conditions is met:
 *        - the state of S is not SOLVING, and no IPASIR2 function is executing on S.
 *        - the state of S is SOLVING, and no other IPASIR2 function except
 *          ipasir2_solve() is executing on S, and a callback function is executing
 *          that has been called by the IPASIR2 implementation due to a call to
 *          ipasir2_solve() on S.
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
 * @var ipasir2_errorcode::IPASIR2_E_UNSUPPORTED_OPTION
 *  @brief Unknown option.
 *  @details The option is not supported by the solver.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_INVALID_STATE
 *  @brief Invalid state.
 *  @details The function call is not allowed in the current state of the solver.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_INVALID_ARGUMENT
 *  @brief Invalid argument.
 *  @details The function call failed because of an invalid argument.
 * 
 * @var ipasir2_errorcode::IPASIR2_E_INVALID_OPTION_VALUE
 *  @brief Invalid option value.
 *  @details The option value is outside the allowed range.
 * 
 */
typedef enum ipasir2_errorcode {
    IPASIR2_E_OK = 0,
    IPASIR2_E_UNKNOWN = 1,
    IPASIR2_E_UNSUPPORTED,
    IPASIR2_E_UNSUPPORTED_ARGUMENT,
    IPASIR2_E_UNSUPPORTED_OPTION,
    IPASIR2_E_INVALID_STATE,
    IPASIR2_E_INVALID_ARGUMENT,
    IPASIR2_E_INVALID_OPTION_VALUE
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
 *      Pragmatics: In presence of an external theory solver, 
 *                  clauses which are hard to derive again are marked as irredundent, 
 *                  such that the solver must keep them.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_FORGETTABLE
 *  @brief Irredundant but forgettable clauses.
 *  @details The clauses served by the import clause callback are not necessarily redundant
 *      and might change the satisfiability of the formula. 
 *      However, the solver is allowed to forget these clauses.
 *      Forgettable clauses might introduce new variables.
 * 
 *      Pragmatics: In presence of an external theory solver,
 *                  clauses which are easy to derive again are marked as forgettable,
 *                  and will be added again if needed.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_EQUISATISFIABLE
 *  @brief Equisatisfiable clauses.
 *  @details The clauses served by the import clause callback are satisfiability preserving.
 *      Satisfiability preserving clauses might introduce new variables.
 * 
 *      Pragmatics: In clause sharing parallel portfolios, derived clauses are not 
 *                  allowed to change the satisfiability of the formula, 
 *                  but might change the models, e.g. if extended resolution 
 *                  or blocked clause elimination are allowed.
 * 
 * @var ipasir2_redundancy::IPASIR2_R_EQUIVALENT
 *  @brief Equivalence preserving clauses.
 *  @details The clauses served by the import clause callback are equivalence preserving.
 *      Equivalence preserving clauses do not introduce new variables.
 * 
 *      Pragmatics: Only clauses that preserve the models of the formula are allowed.
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
 * @var ipasir2_option::name
 *  @brief Unique option identifier.
 *  @details Option identifers can be grouped into namespaces which are separated by a dot.
 *      The IPASIR-2 specification reserves the namespace "ipasir." for options defined by the IPASIR-2 specification.
 *      If a solver provides an option from the "ipasir." namespace, its behavior must be as specified in the IPASIR-2 specification.
 *      If a solver does not support a given option, it returns IPASIR2_E_UNSUPPORTED_OPTION when the option is set.
 * 
 * @var ipasir2_option::min
 *  @brief Minimum value.
 *  @details Minimum allowed value for the option.
 *      The solver must return IPASIR2_E_INVALID_OPTION_VALUE if the option is set to a value outside the range [min, max].
 * 
 * @var ipasir2_option::max
 *  @brief Maximum value.
 *  @details Maximum allowed value for the option.
 *      The solver must return IPASIR2_E_INVALID_OPTION_VALUE if the option is set to a value outside the range [min, max].
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
    int tunable;
    int indexed;
    void const* handle;
} ipasir2_option;


/**
 * @brief Return the name and the version of the incremental SAT solver library.
 * 
 * Thread safety: this function is type A.
 *
 * @param signature After successful execution, the output parameter points to the library name and version string.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 */
IPASIR_API ipasir2_errorcode ipasir2_signature(char const** signature);


/**
 * @brief Construct a new solver instance and set result to return a pointer to it.
 * @details Use the returned pointer as the first parameter in each of the following functions.
 *
 * Thread safety: this function is type A.
 *
 * @param solver After successful execution, the output parameter points to the solver instance.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *
 * Required state of @p *solver: undefined
 * State of @p *solver after the function returns: CONFIG
 */
IPASIR_API ipasir2_errorcode ipasir2_init(void** solver);


/**
 * @brief Release the given solver (destructor). 
 * @details Release all solver resources and allocated memory. 
 *          The solver pointer cannot be used for any purposes after this call.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance to be released.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_INVALID_STATE if the solver is in the SOLVING state.
 *
 * Required state of \p solver: CONFIG <= state < SOLVING
 * State of \p solver after the function returns: undefined
 */
IPASIR_API ipasir2_errorcode ipasir2_release(void* solver);


/** 
 * @brief Return the configuration options which are supported by the solver.
 * @details The array contains all available options for the solver.
 *          The array is owned by the solver and must not be freed by the caller.
 *          Options in the namespace "ipasir." are reserved by IPASIR specification.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param options After successful execution, the output parameter points to a zero-terminated array of ipasir2_options.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not implement the configuration interface.
 * 
 * Required state of \p solver: state <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_options(void* solver, ipasir2_option const** options);


/**
 * @brief Return the handle to the option with the given name.
 * @details The handle can be used to set the option value.
 *          Convenience function for searching the option array returned by ipasir2_options().
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param name The option identifier.
 * @param handle After successful execution, the output parameter points to the option handle.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not implement the configuration interface.
 *         IPASIR2_E_UNSUPPORTED_OPTION if the option is not supported by the solver.
 * 
 * Required state of \p solver: state <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API inline ipasir2_errorcode ipasir2_get_option_handle(void* solver, char const* name, ipasir2_option const** handle) {
    ipasir2_option const* options = NULL;
    ipasir2_errorcode err = ipasir2_options(solver, &options);
    if (err == IPASIR2_E_OK) {
        for (; options->name != NULL; ++options) {
            if (strcmp(options->name, name) == 0) {
                *handle = options;
                return IPASIR2_E_OK;
            }
        }
        return IPASIR2_E_UNSUPPORTED_OPTION;
    } else {
        return err;
    }
}


/** 
 * @brief Set value of option identified by the given handle.
 * @details The option value is set to the given value if the value is within the allowed range, 
 *          and if the solver is in a state in which the option is allowed to be set.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param handle The option handle.
 * @param value The option value to be set.
 * @param index Case distinction:
 *                  - ipasir2_option::indexed == true:
 *                      The \p index contains the variable index for which the option is to be set.
 *                      Use zero if the value should be applied to all variables.
 *                  - ipasir2_option::indexed == false:
 *                      The \p index is ignored and can be set to any value.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_INVALID_OPTION_VALUE if the option value is outside the allowed range.
 *         IPASIR2_E_INVALID_STATE if the option is not allowed to be set in the current state.
 * 
 * Required state of \p solver: <= handle->max_state
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* solver, ipasir2_option const* handle, int64_t value, int64_t index);


/**
 * @brief Add the given literal into the currently added clause or finalize the clause with a 0. 
 * @details Clauses added this way cannot be removed. 
 *          The addition of removable clauses can be simulated using activation literals and assumptions.
 *          Literals are encoded as (non-zero) integers as in the DIMACS formats.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param clause The clause of length \p len to be added.
 * @param len The number of literals in \p clause.
 * @param redundancy The redundancy type of \p clause with respect to the previously added irredundant clauses. 
 *                   The redundancy of \p clause affects its required persistency and 
 *                   its potential impact on solver state consistency.
 *                   This is mostly relevent when used from the import-callback, e.g., 
 *                   in the context of parallel SAT solver frameworks or 
 *                   in case of lazily encoded background theories.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED_ARGUMENT if the redundancy type is generally not supported.
 *         IPASIR2_E_INVALID_STATE if the redundancy type is not supported in the present state.
 * 
 * Required state of \p solver: state <= SOLVING
 * State of \p solver after the function returns: if state < SOLVING then INPUT else SOLVING
 */
IPASIR_API ipasir2_errorcode ipasir2_add(void* solver, int32_t const* clause, int32_t len, ipasir2_redundancy redundancy);


/**
 * @brief Solve the formula with specified clauses under the given assumption \p literals.
 * @details If the formula is satisfiable, the output parameter \p result is set to 10 and the state of the solver is changed to SAT.
 *          If the formula is unsatisfiable, the output parameter \p result is set to 20 and the state of the solver is changed to UNSAT.
 *          If the search is interrupted, the output parameter \p result is set to 0 and the state of the solver is changed to INPUT.
 *          The state of the solver during execution of ipasir2_solve() is SOLVING.
 *          If the solver calls any of the callback functions during execution of ipasir2_solve(), the state of the solver is SOLVING as well.
 *          Callbacks are allowed to call any ipasir2 function which is allowed in the SOLVING state.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param literals Array of assumptions literals (can be nullptr in case of no assumptions).
 * @param len The number of assumptions in \p literals (zero if \p literals is nullptr).
 * @param result After successful execution, the output parameter points to the result of the SAT search.
 *               The result is one of the following values:
 *                  - 0: The search was interrupted.
 *                  - 10: The formula is satisfiable.
 *                  - 20: The formula is unsatisfiable.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_INVALID_STATE if the solver is in the SOLVING state.
 *
 * Required state of \p solver: CONFIG <= state < SOLVING
 * State of \p solver after the function returns: INPUT or SAT or UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_solve(void* solver, int* result, int32_t const* literals, int32_t len);


/**
 * @brief Return the truth value of the given literal in the found satisfying assignment.
 * @details The function can only be used if the solver is in state SAT. 
 *          This means that ipasir2_solve() has returned 10 and no ipasir2_add nor ipasir2_assume has been called since then.
 *          The output parameter \p result is set to 'lit' if \p lit is satisfied by the model, 
 *          and is set to '-lit' if \p lit is not satisfied by the model.
 *          The output parameter \result may be set to zero if the found assignment is satisfying for both 'lit' and '-lit'.
 *          Each solution that agrees with all non-zero values of ipasir2_val() is a model of the formula.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param lit The literal whose truth value is to be returned.
 * @param result After successful execution, the output parameter is set to the truth value of the literal.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_INVALID_STATE if the solver is not in the SAT state.
 *         IPASIR2_E_INVALID_ARGUMENT if the literal is not valid.
 *
 * Required state of \p solver: SAT
 * State of \p solver after the function returns: SAT
 */
IPASIR_API ipasir2_errorcode ipasir2_val(void* solver, int32_t lit, int32_t* result);


/**
 * @brief Check if the given assumption literal was used to prove the unsatisfiability in the last SAT search.
 * @details The function can only be used if the solver is in state UNSAT.
 *          This means that ipasir2_solve() has returned 20 and no ipasir2_add nor ipasir2_assume has been called since then.
 *          The literal \p lit must be one of the assumption literals used in the last SAT search.
 *          The output parameter \p result is set to 1 if the given assumption literal was used to prove unsatisfiability,
 *          and is set to 0 otherwise. The set of assumptions literals for which \result is 1 by this function call 
 *          forms a (not necessarily minimal) unsatisfiable core for the formula. This means the formula is unsatisfiable 
 *          under this subset of the previously used assumption literals. 
 *
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param lit The assumption literal.
 * @param result After successful execution, the output parameter is set to 1 
 *               if the given assumption literal was used to prove unsatisfiability and set to 0 otherwise.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_INVALID_STATE if the solver is not in the UNSAT state.
 *         IPASIR2_E_INVALID_ARGUMENT if the literal is not a valid assumption literal from the last call.
 * 
 * Required state of \p solver: UNSAT
 * State of \p solver after the function returns: UNSAT
 */
IPASIR_API ipasir2_errorcode ipasir2_failed(void* solver, int32_t lit, int* result);


/**
 * @brief Set a callback function used to indicate a termination requirement to the solver.
 * @details The solver periodically calls this function while being in SOLVING state.
 *          If the callback function returns a non-zero value, the solver terminates search.
 *          If this function is called multiple times on \p solver, only the most recent call is considered.
 *          When the callback function is called for \p solver, the \p data argument given in this call is passed
 *          to the callback as its first argument.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param data Opaque pointer passed to the callback function as the first parameter. May be nullptr.
 * @param callback The terminate callback function with the same signature as "int terminate(void* data)".
 *                 If this parameter is nullptr, this callback mechanism is disabled until the next call to this function.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not support termination callbacks.
 *
 * Required state of \p solver: <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, 
    int (*callback)(void* data));


/**
 * @brief Set a callback function for receiving learned clauses from the solver. 
 * @details The solver calls this function for each learned clause of size <= \p max_length.
 *          The solver calls this function for each learned clause regardless of its size if \p max_length is -1.
 *          The \p clause argument passed to the callback is a pointer to a zero terminated integer array containing
 *          the learned clause. \p clause is only guaranteed to be valid during the execution of the callback function.
 *          If this function is called multiple times on \p solver, only the most recent call is considered.
 *          When the callback function is called for \p solver, the \p data argument given in this call is passed
 *          to the callback as its first argument.
 * 
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param data Opaque pointer passed to the callback function as the first parameter. May be nullptr.
 * @param max_length Specifies the maximum length of the learned clauses to be returned. 
 *                   If this parameter is -1 the solver returns all learned clauses.
 * @param callback The clause export callback function with the same signature as "void callback(void* data, int32_t const* clause)".
 *                 If this parameter is nullptr, this callback mechanism is disabled until the next call to this function.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not support clause export callbacks.
 * 
 * Required state of \p solver: <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_export(void* solver, void* data, int max_length, 
    void (*callback)(void* data, int32_t const* clause));


/**
 * @brief Sets a callback for asynchronously sending clauses to the solver. 
 * @details The solver calls this function periodically while being in SOLVING state.
 *          If this function is called multiple times on \p solver, only the most recent call is considered.
 *          When the callback function is called for \p solver, the \p data argument given in this call is passed
 *          to the callback as its first argument.
 *
 *          Applications give a \p pledge about the minimum redundancy type of the imported clauses.
 *          Solvers can reject too weak redundancy types by returning IPASIR2_E_UNSUPPORTED_ARGUMENT.
 *          The redundancy type of any imported clause must be at least as strong pledged.
 *          The callback function sets \p type to the actual redundancy type of the clause.
 *
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param data Opaque pointer passed to the callback function as the first parameter. May be nullptr.
 * @param pledge Promise on the minimum redundancy type of the clauses to be imported.
 * @param callback The clause import callback function with the same signature as "void callback(void* data, ipasir2_redundancy min)".
 *                 The callback() uses ipasir2_add() at most once to import a clause of the redundancy type given by \p min.
 *                 To import more than one clause, the callback() must be called multiple times.
 *                 If there is no further clause of the given redundancy type to be imported, the callback() returns without calling ipasir2_add().
 *                 If this parameter is nullptr, this callback mechanism is disabled until the next call to this function.
 * 
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not support clause import callbacks.
 *         IPASIR2_E_UNSUPPORTED_ARGUMENT if the solver does not support importing clauses 
 *              of redundancy type as low as the given \p pledge.
 *
 * Required state of \p solver: <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_import(void* solver, void* data, ipasir2_redundancy pledge, 
    void (*callback)(void* data, ipasir2_redundancy min));


/**
 * @brief Sets a callback to notify about changes in the current partial assignment under analysis.
 * @details The solver calls this function periodically while being in SOLVING state.
 *          Changes are returned for all variables that have been assigned or unassigned since the last call to the callback.
 *          The solver must ensure that variables in \p assigned and \p unassigned are non-intersecting.
 *          This rule has implication on the minimum frequency of calls to the \p callback.
 *          All assignments must be reported to the callback before the solver switches to a different state.
 *          In particular, the solver must ensure that all assignments are reported before leaving SOLVING state.
 *          If this function is called multiple times on \p solver, only the most recent call is considered.
 *          When the callback function is called for \p solver, the \p data argument given in this call is passed
 *          to the callback as its first argument.
 *
 * Thread safety: this function is type B.
 *
 * @param solver The solver instance.
 * @param data Opaque pointer passed to the callback function as the first parameter. May be nullptr.
 * @param callback The notify callback function with the same signature as "void callback(void* data, int32_t const* assigned, int32_t const* unassigned)".
 *                 If this parameter is nullptr, this callback mechanism is disabled until the next call to this function.
 *
 * @return IPASIR2_E_OK if the function call was successful.
 *         IPASIR2_E_UNSUPPORTED if the solver does not support notify callbacks.
 * 
 * Required state of \p solver: <= SOLVING
 * State of \p solver after the function returns: same as before
 */
IPASIR_API ipasir2_errorcode ipasir2_set_notify(void* solver, void* data, 
    void (*callback)(void* data, int32_t const* assigned, int32_t const* unassigned));

#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR2_H_
