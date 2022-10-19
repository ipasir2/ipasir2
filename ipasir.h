/* Part of the generic incremental SAT API called 'ipasir'.
 * See 'LICENSE' for rights to use this software.
 */
#ifndef INTERFACE_IPASIR_H_
#define INTERFACE_IPASIR_H_

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
 * ##### 
 * ##### (The following additional major contributions could be included:
 * ##### - Solver Stats:    A generic interface for extracting solver statistics)
 * #####
 * ##### The following minor contributions are included:
 * ##### - Versioning:      A constant indication the ipasir version
 * ##### 
 * ##### (The following additional minor contributions could be included:
 * ##### - Clause Deletion: A callback interface for signalling deletion of clauses)
 * #################################################################################################################
 */



/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Indicate IPASIR Version
 */
#define IPASIR_VERSION 2



/**
 * #################################################################################################################
 * ##### The IPASIR 2.0 SAT Solver Configuration Interface
 * #####
 * ##### The following aspects have been respected in the desgin of the configuration interface:
 * ##### 1. Genericity: The configuration interface can be used for all incremental SAT solvers and their applications.
 * #####    This is achieved by using a generic configuration key-value pair interface with introspection capabilities.
 * ##### 2. Simplicity: The configuration interface is easy to use.
 * #####    This is achieved by specifying must-be-supported configuration keys and namespaces.
 * ##### 3. Extensibility: The configuration interface can be used for future extensions.
 * #####    This is achieved by using an iterable key-value pair interface.
 * ##### 4. Automatability: The configuration interface can be used by automatic configuration tools and fuzzers.
 * #####    This is achieved by using a generic type-system for introspection which includes value ranges.
 * #################################################################################################################
 */



// Used in ipasir_option_type_t for specifying max/min values
// TODO: Discuss discarding this union and instead using int values for max/min values for all types
typedef union {
    int int_;
    float float_;
} int_or_float;

// TODO: Discuss discarding char
// TODO: Discuss replacing enum with int plus a flag for specifying whether the value ordering carries semantics or not
typedef enum {
    ENUM = 0,
    INT = 1,
    FLOAT = 2,
    CHAR = 3
} ipasir_option_types;

/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Possible types of an ipasir_option
 */
typedef struct {
    /// @brief type identifier
    ipasir_option_types type;

    /// @brief specifies minimum value of an enum, int or float option
    int_or_float minimum;

    /// @brief specifies maximum value of an enum, int or float option
    int_or_float maximum;

    /// @brief specifies whether the option is an array and if so, the length of the array
    ///        (0 means the option is an array of length equal to the number of variables in the formula)
    ///        (1 means the option is not an array)
    ///        (k > 1 means the option is an array of length equal k)
    int length;
} ipasir_option_type_t;


/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Generic IPASIR Options
 */
typedef struct {
    /// @brief Name of the option
    char const* name;

    /// @brief Value of the option
    void const* value;

    /// @brief Type of the option
    ipasir_option_type_t type;
} ipasir_option_t;


/**
 * Examples and Suggested IPASIR Options
 * 
 * random seed for random number generator, e.g., for solver interanal diversification
 *  ipasir.random_seed; type: int; default: 0; min: 0; max: INT32_MAX; length: 1
 * 
 * 
 * TODO: Discuss enum for solving modes:
 * 
 * one-shot-solving mode, disable incremental solving (for activation of solving strategies that are only applicable to one-shot solving)
 *  ipasir.one_shot_solving; type: int; default: 0; min: 0; max: 1; length: 1
 * 
 * simple solving mode, enable low-overhead solving strategies 
 *  ipasir.simple_solving; type: int; default: 0; min: 0; max: 1; length: 1
 * 
 * 
 * TODO: Discuss option for specifying maximum variable id which is not used as assumption (while all greater variable ids are used as assumptions):
 * 
 * a special mode of glucose allows for optimizing activation literals; this requires to specify the number of instance variables
 *  ipasir.max_instance_variables; type: int; default: 0; min: 0; max: INT32_MAX; length: 1
 * 
 * TODO: Discuss standardiying configuration options for diverse solver modules: branching, preprocessing, restarts, learning, forgetting, ...
 * 
 * configuration of variable phases, e.g., positive, negative, etc.
 *  ipasir.branching.variable_phase; type: enum; default: 0; min: 0; max: 1; length: 0
 * 
 * configuration of branching heuristics, e.g., VSIDS, Jeroslow-Wang, etc.
 *  ipasir.branching.vsids.decay; type: float; default: 0.95; min: 0.0; max: 1.0; length: 1
 *  ipasir.branching.vsids.inc; type: float; default: 1.0; min: 0.0; max: 1.0; length: 1
 * 
 * confguration of restart heuristics, e.g., Luby, geometric, etc.
 *  ipasir.restart.luby.init; type: int; default: 100; min: 0; max: INT32_MAX; length: 1
 *
 * configuration of conflict clause deletion heuristics, e.g., LBD, etc.
 *  ipasir.conflict_clause_deletion.core_lbd_max; type: int; default: 3; min: 0; max: INT32_MAX; length: 1
 */

/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Return IPASIR Configuration Options
 * 
 * ipasir_options() returns a NULL terminated array of ipasir_option_t.
 * 
 * The array contains all available options for the solver.
 * The array is owned by the solver and must not be freed by the caller.
 * Its entries are const and must not be modified by the caller.
 * Use the ipasir_set_option() method to change options. 
 * 
 * The array must contain all options as specified in the IPASIR 2 specification.
 * Options with the prefix "ipasir_" are reserved for the IPASIR 2 specification.
 * TODO: specification of IPASIR 2 options
 * TODO: specification of namespaces
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 * 
 * @return ipasir_option_t* Start of array of options
 */
IPASIR_API ipasir_option_t const* ipasir_options(void* S);

/** 
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Set given IPASIR Configuration Option
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API void ipasir_set_option(void* S, ipasir_option_t* opt);


/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Asynchronous Import of Learned Clauses
 * 
 * Set a callback which the internal solver may call while inside ipasir_solve for importing redundant clauses (like “consume” in Lingeling). 
 * The application has the responsibility to appropriately buffer redundant clauses until the solver decides to import (some of) them via the defined callback. 
 * 
 * Effect of Callback:
 *  - literals* points to the next learned clause (zero-terminated like in ipasir_set_learn and ipasir_add)
 *  - literals* points to nullptr if there is no clause to consume
 *  - meta-data* points to the glue value (or sth. else?) of the returned clause (0 < glue <= size); sth. like quality or weight
 *  - Both data* and meta-data* pointers must be valid until the callback is called again or the solver returns from solve
 */
IPASIR_API void ipasir_set_import_redundant_clause(void* solver,
  void (*callback)(void* solver, int** literals, void* meta_data), void* state);



/// Further Suggested IPASIR 2 Methods
/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Provide a standardized way to return solver statistics
 */
typedef struct {
    uint64_t conflicts;
    uint64_t decisions;
    uint64_t propagations;
    uint64_t restarts;
    uint64_t learned_clauses;
    uint64_t deleted_clauses;
} ipasir_stats_t;

IPASIR_API ipasir_stats_t const* ipasir_get_stats();

/// Further Suggested IPASIR 2 Methods
/**
 * IPASIR 2.0: This is new in IPASIR 2.0
 * 
 * @brief Set callback to listen to clause deletions
 */
IPASIR_API void ipasir_set_delete(void* solver, void* data, int max_length, void (*callback)(void* data, int32_t* clause));


/**************************************************************************/
/************************** IPASIR 1 Land begins **************************/
/**************************************************************************/

/**
 * @brief Return the name and the version of the incremental SAT solver library.
 * 
 * @return const char* Library name and version
 */
IPASIR_API const char* ipasir_signature();

/**
 * @brief Construct a new solver instance and return a pointer to it.
 * 
 * Use the returned pointer as the first parameter in each of the following functions.
 *
 * @return void* SAT solver
 *
 * Required state: undefined
 * State after: INPUT
 */
IPASIR_API void* ipasir_init();

/**
 * @brief Release the given solver (destructor). 
 * 
 * Release all solver resources and allocated memory. 
 * The solver pointer cannot be used for any purposes after this call.
 * 
 * @param solver SAT solver
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: undefined
 */
IPASIR_API void ipasir_release(void* solver);

/**
 * @brief Add the given literal into the currently added clause or finalize the clause with a 0. 
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
IPASIR_API void ipasir_add(void* solver, int32_t lit_or_zero);

/**
 * @brief Add an assumption for the next SAT search. 
 * 
 * The assumption will be used in the next call of ipasir_solve(). 
 * After calling ipasir_solve() all the previously added assumptions are cleared.
 * 
 * @param solver SAT solver
 * @param lit Assumption Literal
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT
 */
IPASIR_API void ipasir_assume(void* solver, int32_t lit);

/**
 * @brief Solve the formula with specified clauses under the specified assumptions.
 * 
 * If the formula is satisfiable the function returns 10 
 * and the state of the solver is changed to SAT. 
 * If the formula is unsatisfiable the function returns 20 
 * and the state of the solver is changed to UNSAT. 
 * If the search is interrupted the function returns 0 
 * and the state of the solver is changed to INPUT (cf. ipasir_set_terminate()). 
 * This function can be called in any defined state of the solver. 
 * Note that the state of the solver _during_ execution of 'ipasir_solve' is undefined.
 * 
 * @param solver SAT solver
 * @return int 10, 20 or 0
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API int ipasir_solve(void* solver);

/**
 * @brief Return the truth value of the given literal in the found satisfying assignment.
 * 
 * Return 'lit' if True, '-lit' if False; 'ipasir_val(lit)'
 * may return '0' if the found assignment is satisfying for both
 * valuations of lit. 
 * 
 * Each solution that agrees with all non-zero values of 
 * ipasir_val() is a model of the formula.
 *
 * This function can only be used if ipasir_solve() has returned 10
 * and no 'ipasir_add' nor 'ipasir_assume' has been called
 * since then, i.e., the state of the solver is SAT.
 * 
 * @param solver SAT solver
 * @param lit Literal
 * @return int32_t Truth value of the given literal
 *
 * Required state: SAT
 * State after: SAT
 */
IPASIR_API int32_t ipasir_val(void* solver, int32_t lit);

/**
 * @brief Check if the given assumption literal was used to prove the
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
 * @param solver 
 * @param lit 
 * @return int
 * 
 * Required state: UNSAT
 * State after: UNSAT
 */
IPASIR_API int ipasir_failed(void* solver, int32_t lit);

/**
 * @brief Set a callback function used to indicate a termination requirement to the solver.
 * 
 * The solver will periodically call this function and
 * check its return value during the search.  The ipasir_set_terminate
 * function can be called in any state of the solver, the state remains
 * unchanged after the call.  The callback function is of the form
 * "int terminate(void * data)"
 *   - it returns a non-zero value if the solver should terminate.
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the ipasir_set_terminate function (2nd parameter).
 * 
 * @param solver 
 * @param data 
 * @param terminate 
 *
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API void ipasir_set_terminate(void* solver, void* data, int (*terminate)(void* data));

/**
 * @brief Set a callback function for extracting learned clauses up to a given length from the solver. 
 *  
 * The solver will call this function for each learned clause 
 * that satisfies the maximum length (literal count) condition. 
 * 
 * The ipasir_set_learn function can be called in any state of the solver, 
 * the state remains unchanged after the call. 
 * 
 * The callback function is of the form "void learn(void* data, int* clause)"
 *   - the solver calls the callback function with the parameter "data"
 *     having the value passed in the 2nd parameter of the ipasir_set_learn() function.
 *   - the argument "clause" is a pointer to a null terminated integer array 
 *     containing the learned clause. 
 *   - the solver can change the memory where "clause" points to 
 *     after the function call.
 *   - the solver calls the callback function from the same thread
 *     in which ipasir_solve() has been called.
 * 
 * Subsequent calls to ipasir_set_learn() override the previously set callback function. 
 * Setting the callback function to NULL with any max_length argument disables the callback.
 * 
 * @param solver SAT solver
 * @param data 
 * @param max_length 
 * @param learn 
 * 
 * Required state: INPUT or SAT or UNSAT
 * State after: INPUT or SAT or UNSAT
 */
IPASIR_API void ipasir_set_learn(void* solver, void* data, int max_length, void (*callback)(void* data, int32_t* clause));

#ifdef __cplusplus
}  // closing extern "C"
#endif

#endif  // INTERFACE_IPASIR_H_
