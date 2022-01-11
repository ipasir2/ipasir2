/* Part of the generic incremental SAT API called 'ipasir'.
 * See 'LICENSE' for rights to use this software.
 */
#ifndef ipasir_h_INCLUDED
#define ipasir_h_INCLUDED

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


/**************************************************************************/
/************************** IPASIR 2 Land begins **************************/
/**************************************************************************/


/**
 * @brief Indicate IPASIR Version
 * 
 * Can become useful for applications to determine availabilty of methods.
 * 
 * Suggested Backport: Use 1 for initial version and 1.5 for versions with ipasir_set_learned_callback()
 */
#define IPASIR_VERSION 2


/**
 * IPASIR Configuration
 * 
 * Restricted to options in the specified configuration struct. 
 * 
 * TODO: Specify when configuration is allowed to take place
 * TODO: Specify feature categories to be used as prefix (core_, vsids_, etc.)
 * 
 * Some initial ideas:
 */
struct ipasir_config_t {
    int seed;  // Solver internal diversification
    bool initial_polarity;  // negative branching
    char* initial_phases;  // more powerful, would subsume global initial_polarity
    bool non_incremental;  // https://github.com/biotomas/ipasir/pull/5
    bool simple_mode;  // (cheap mode?) or so…
    int max_var;  // when all above are assumptions (activate special operation mode of Glucose)
};

/**
 * @brief Return IPASIR Configuration
 * 
 * Returns the current solver configuration. 
 * If none has been set before, the configuration is filled with the correct solver defaults. 
 * The memory of the returned configuration struct belongs to the solver.
 * 
 * 
 * Proposed Usage:
 * 
 *   ipasir_config_t* config = ipasir_get_configuration(S);  // construct
 *   config.feature = value;  // modify
 *   ...
 *   ipasir_set_configuration(S, config);  // set
 * 
 * 
 * This should be an involution: 
 * 
 *   ipasir_set_configuration(ipasir_get_configuration(S))
 * 
 * @param S SAT Solver
 * @return ipasir_config_t* Solver Configuration
 */
ipasir_config_t* ipasir_get_configuration(void* S);

/**
 * @brief Set IPASIR Configuration
 * 
 * @param S SAT Solver
 * @param config Solver Configuration
 */
void ipasir_set_configuration(void* S, ipasir_config_t* config);


/**
 * @brief Asynchronous Import of Learned Clauses
 * 
 * Set a callback which the internal solver may call while inside ipasir_solve for importing redundant clauses (like “consume” in Lingeling). 
 * The application has the responsibility to appropriately buffer redundant clauses until the solver decides to import (some of) them via the defined callback. 
 * 
 * Effect of Callback:
 *  - data* points to the next learned clause (zero-terminated like in ipasir_set_learn and ipasir_add)
 *  - data* points to nullptr if there is no clause to consume
 *  - glue* points to the glue value of the returned clause (0 < glue <= size)
 *  - Both data* and glue* pointers must be valid until the callback is called again or the solver returns from solve
 */
void ipasir_set_import_redundant_clause(void* solver,
  void (*callback)(void* solver, int** data, int* glue), void* state);


/**************************************************************************/
/************************** IPASIR ? Land begins **************************/
/**************************************************************************/

/**
 * @brief Provide a standardized way to return solver statistics
 */
struct ipasir_stats_t {
    uint64_t conflicts;
    uint64_t decisions;
    uint64_t propagations;
};

ipasir_stats_t* ipasir_get_stats();


/**
 * @brief Set callback to listen to clause deletions
 */
void ipasir_set_delete(void* solver, void* data, int max_length, void (*learn)(void* data, int32_t* clause));


/**
 * @brief Improve learned callback:
 *  - either remove max_length (breaking change, use-case for IPASIR_VERSION)
 *  - or allow negative values to disable max_length
 */
void ipasir_set_learn (void * solver, void * data, int max_length, void (*learn)(void * data, int32_t * clause));


/**************************************************************************/
/************************** IPASIR 1 Land begins **************************/
/**************************************************************************/

/**
 * Return the name and the version of the incremental SAT
 * solving library.
 */
IPASIR_API const char * ipasir_signature ();

/**
 * Construct a new solver and return a pointer to it.
 * Use the returned pointer as the first parameter in each
 * of the following functions.
 *
 * Required state: N/A
 * State after: INPUT
 */
IPASIR_API void * ipasir_init ();

/**
 * Release the solver, i.e., all its resoruces and
 * allocated memory (destructor). The solver pointer
 * cannot be used for any purposes after this call.
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: undefined
 */
IPASIR_API void ipasir_release (void * solver);

/**
 * Add the given literal into the currently added clause
 * or finalize the clause with a 0.  Clauses added this way
 * cannot be removed. The addition of removable clauses
 * can be simulated using activation literals and assumptions.
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 *
 * Literals are encoded as (non-zero) integers as in the
 * DIMACS formats.  They have to be smaller or equal to
 * INT32_MAX and strictly larger than INT32_MIN (to avoid
 * negation overflow).  This applies to all the literal
 * arguments in API functions.
 */
IPASIR_API void ipasir_add (void * solver, int32_t lit_or_zero);

/**
 * Add an assumption for the next SAT search (the next call
 * of ipasir_solve). After calling ipasir_solve all the
 * previously added assumptions are cleared.
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API void ipasir_assume (void * solver, int32_t lit);

/**
 * Solve the formula with specified clauses under the specified
 * assumptions.  If the formula is satisfiable the function returns 10
 * and the state of the solver is changed to SAT.  If the formula is
 * unsatisfiable the function returns 20 and the state of the solver is
 * changed to UNSAT.  If the search is interrupted (see
 * ipasir_set_terminate) the function returns 0 and the state of the
 * solver is changed to INPUT.  This function can be called in any
 * defined state of the solver.  Note that the state of the solver
 * _during_ execution of 'ipasir_solve' is undefined.
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API int ipasir_solve (void * solver);

/**
 * Get the truth value of the given literal in the found satisfying
 * assignment.  Return 'lit' if True, '-lit' if False; 'ipasir_val(lit)'
 * may return '0' if the found assignment is satisfying for both
 * valuations of lit.  Each solution that agrees with all non-zero
 * values of ipasir_val() is a model of the formula.
 *
 * This function can only be used if ipasir_solve has returned 10
 * and no 'ipasir_add' nor 'ipasir_assume' has been called
 * since then, i.e., the state of the solver is SAT.
 *
 * Required state: SAT
 * State after: SAT
 */
IPASIR_API int32_t ipasir_val (void * solver, int32_t lit);

/**
 * Check if the given assumption literal was used to prove the
 * unsatisfiability of the formula under the assumptions
 * used for the last SAT search. Return 1 if so, 0 otherwise.
 *
 * The formula remains unsatisfiable even just under assumption literals
 * for which ipasir_failed() returns 1.  Note that for literals 'lit'
 * which are not assumption literals, the behavior of
 * 'ipasir_failed(lit)' is not specified.
 *
 * This function can only be used if ipasir_solve has returned 20 and
 * no ipasir_add or ipasir_assume has been called since then, i.e.,
 * the state of the solver is UNSAT.
 *
 * Required state: UNSAT
 * State after: UNSAT
 */
IPASIR_API int ipasir_failed (void * solver, int32_t lit);

/**
 * Set a callback function used to indicate a termination requirement to
 * the solver.  The solver will periodically call this function and
 * check its return value during the search.  The ipasir_set_terminate
 * function can be called in any state of the solver, the state remains
 * unchanged after the call.  The callback function is of the form
 * "int terminate(void * data)"
 *   - it returns a non-zero value if the solver should terminate.
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the ipasir_set_terminate function
 *     (2nd parameter).
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API void ipasir_set_terminate (void * solver, void * data, int (*terminate)(void * data));

/**
 * Set a callback function used to extract learned clauses up to a given
 * length from the solver.  The solver will call this function for each
 * learned clause that satisfies the maximum length (literal count)
 * condition.  The ipasir_set_learn function can be called in any state
 * of the solver, the state remains unchanged after the call.  The
 * callback function is of the form
 * "void learn(void * data, int * clause)"
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the ipasir_set_learn function
 *     (2nd parameter).
 *   - the argument "clause" is a pointer to a null terminated integer
 *     array containing the learned clause. the solver can change the
 *     data at the memory location that "clause" points to after the
 *     function call.
 *   - the solver calls the callback function from the same thread
 *     in which ipasir_solve has been called.
 *
 * Subsequent calls to ipasir_set_learn override the previously
 * set callback function.  Setting the callback function to NULL
 * with any max_length argument disables the callback.
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API void ipasir_set_learn (void * solver, void * data, int max_length, void (*learn)(void * data, int32_t * clause));

#ifdef __cplusplus
} // closing extern "C"
#endif

#endif
