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
 * ##### IPASIR 2.0 Extension Draft --- New functionality is marked with "IPASIR 2.0" in the comments.
 * #####
 * ##### The following major contributions are included:
 * ##### - Configuration:   A generic configuration interface for incremental SAT solvers
 * ##### - Clause Sharing:  A callback interface for asynchronous import of shared clauses
 * ##### - Error Codes:     All function return an error code
 * ##### 
 * ##### (The following additional major contributions could be included:
 * ##### - Solver Stats:    A generic interface for extracting solver statistics)
 * #####
 * #################################################################################################################
 */


/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief IPASIR 2.0 Error Codes
 */
typedef enum {
  IPASIR_E_OK = 0,
  IPASIR_E_UNKNOWN = 1, // to be used if no other code applies
  IPASIR_E_UNSUPPORTED = 2,
  IPASIR_E_OUT_OF_TIME = 3,
  IPASIR_E_OUT_OF_MEM = 4,
  IPASIR_E_INVALID_CONFIG = 5,
  IPASIR_E_IO = 6,
  IPASIR_E_INVALID_STATE = 7, // to be used if a function is called in a state which is not allowed by the ipasir state machine
  IPASIR_E_OPTION_UNKNOWN = 8,
} ipasir2_errorcode;


/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Possible types of configuration options
 */
typedef enum {
    ENUM = 0,
    INT = 1,
    FLOAT = 2
} ipasir2_option_type;

/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * TODO: describe identifier naming schema here
 * 
 * @brief Specification of options for the configuration interface
 */
typedef struct {
    /// identifier for the option
    char const* name;

    /// @brief type identifier
    ipasir2_option_type type;

    /// @brief minimum value
    int minimum;

    /// @brief maximum value
    int maximum;

    /// @brief flag inicating whether the option is an array:
    ///        - 0: the option is not an array
    ///        - 1: the option is an array of length equal to the number of variables in the formula
    int is_array;
} ipasir2_option;


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
IPASIR_API ipasir2_errorcode ipasir2_options(void* S, ipasir2_option const** result);

/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Set given IPASIR Configuration Option
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API ipasir2_errorcode ipasir2_set_option(void* S, char const* name, void const* value);


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
  void (*callback)(void* solver, int** literals, void* meta_data), void* state);


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
IPASIR_API ipasir2_errorcode ipasir2_signature(char const** result);

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
IPASIR_API ipasir2_errorcode ipasir2_init(void** result);

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
IPASIR_API ipasir2_errorcode ipasir2_release(void* solver);

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
IPASIR_API ipasir2_errorcode ipasir2_add(void* solver, int32_t lit_or_zero);

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
IPASIR_API ipasir2_errorcode ipasir2_assume(void* solver, int32_t lit);

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
IPASIR_API ipasir2_errorcode ipasir2_solve(void* solver, int* result);

/**
 * @brief Return the number of variables on the solver's assignment stack.
 * 
 * New in IPASIR 2.0.
 * 
 * This function can only be used if IPASIR2 is in either SAT or INPUT state.
 * In SAT state, the number of variables on the assignment stack is 
 * the number of variables in the formula.
 * In INPUT state, the number of variables on the assignment stack is 
 * the number of variables in the current partial assignment, e.g., if
 * a conflict limit was reached during search.
 * 
 * @param solver SAT solver
 * @param &result Number of variables on the assignment stack
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT
 * State after: INPUT or SAT
 */

IPASIR_API ipasir2_errorcode ipasir2_assignment_size(void* solver, int32_t* result);

/**
 * @brief Return the assignment at the given position on the solver's assignment stack.
 * 
 * New in IPASIR 2.0.
 * 
 * This function can only be used if IPASIR2 is in either SAT or INPUT state.
 * The assignment stack is indexed from 0 to assignment_size - 1.
 * 
 * @param solver SAT solver
 * @param index Index of the assignment on the assignment stack
 * @param &result Assignment at the given position on the assignment stack
 * @return ipasir2_errorcode
 * 
 * Required state: INPUT or SAT
 * State after: INPUT or SAT
 */

IPASIR_API ipasir2_errorcode ipasir2_assignment(void* solver, int32_t index, int32_t* result);

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
IPASIR_API ipasir2_errorcode ipasir2_val(void* solver, int32_t lit, int32_t* result);

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
IPASIR_API ipasir2_errorcode ipasir2_failed(void* solver, int32_t lit, int* result);

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
IPASIR_API ipasir2_errorcode ipasir2_set_terminate(void* solver, void* data, int (*terminate)(void* data));

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
IPASIR_API ipasir2_errorcode ipasir2_set_learn(void* solver, void* data, void (*callback)(void* data, int32_t* clause));

#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR2_H_
