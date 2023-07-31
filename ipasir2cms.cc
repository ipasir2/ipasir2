/**
 * @file ipasir2cms.cc
 * @author Markus Iser (markus.iser@kit.edu)
 * @brief Wrap CryptoMinisat solver into IPASIR 2 solver
 * @version 0.1
 * @date 2022-11-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ipasir.h"
#include "ipasir2.h"

#include "cryptominisat.h"
#include <vector>
#include <complex>
#include <cassert>
#include <string.h>
#include "constants.h"

#include "solverconf.h"

using std::vector;
using namespace CMSat;
struct MySolver {
    ~MySolver()
    {
        delete solver;
    }

    MySolver()
    {
        solver = new SATSolver;
    }

    SATSolver* solver;
    vector<Lit> clause;
    vector<Lit> assumptions;
    vector<Lit> last_conflict;
    vector<char> conflict_cl_map;
};

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
IPASIR_API ipasir2_errorcode ipasir2_options(void* S, ipasir2_option const** result) {
    ipasir2_option* solver_options = new ipasir2_option[23];
    solver_options[0] = { "branch_strategy_setup", ipasir2_option_type::INT, 0, 1 };
    solver_options[1] = { "varElimRatioPerIter", ipasir2_option_type::FLOAT, 0.1, 1 };
    solver_options[2] = { "restartType", ipasir2_option_type::INT, 0, 4 };
    solver_options[3] = { "polarity_mode", ipasir2_option_type::INT, 0, 7 };
    solver_options[4] = { "inc_max_temp_lev2_red_cls", ipasir2_option_type::FLOAT, 1.0, 1.04 };
    solver_options[5] = { "clause_clean_glue", ipasir2_option_type::FLOAT, 0, 0.5 };
    solver_options[6] = { "clause_clean_activity", ipasir2_option_type::FLOAT, 0, 0.5 };
    solver_options[7] = { "glue_put_lev0_if_below_or_eq", ipasir2_option_type::INT, 0, 4 };
    solver_options[8] = { "glue_put_lev1_if_below_or_eq", ipasir2_option_type::INT, 0, 6 };
    solver_options[9] = { "every_lev1_reduce", ipasir2_option_type::INT, 1, 10000 };
    solver_options[10] = { "every_lev2_reduce", ipasir2_option_type::INT, 1, 15000 };
    solver_options[11] = { "do_bva", ipasir2_option_type::INT, 0, 1 };
    solver_options[12] = { "max_temp_lev2_learnt_clauses", ipasir2_option_type::INT, 10000, 30000 };
    solver_options[13] = { "never_stop_search", ipasir2_option_type::INT, 0, 1 };
    solver_options[14] = { "doMinimRedMoreMore", ipasir2_option_type::INT, 0, 2 };
    solver_options[15] = { "max_num_lits_more_more_red_min", ipasir2_option_type::INT, 0, 20 };
    solver_options[16] = { "max_glue_more_minim", ipasir2_option_type::INT, 0, 4 };
    solver_options[17] = { "orig_global_timeout_multiplier", ipasir2_option_type::INT, 0, 5 };
    solver_options[18] = { "num_conflicts_of_search_inc", ipasir2_option_type::FLOAT, 1, 1.15 };
    solver_options[19] = { "more_red_minim_limit_binary", ipasir2_option_type::INT, 0, 600 };
    solver_options[20] = { "restart_inc", ipasir2_option_type::FLOAT, 1.1, 1.5 };
    solver_options[21] = { "restart_first", ipasir2_option_type::INT, 100, 500 };
    solver_options[22] = { 0 };
    *result = solver_options;
    return IPASIR_E_OK;
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
    MySolver* s = (MySolver*)S;

    if (strcmp(name, "branch_strategy_setup") == 0) {
        return IPASIR_E_OK;
    } 
    else if (strcmp(name, "varElimRatioPerIter") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "restartType") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "polarity_mode") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "inc_max_temp_lev2_red_cls") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "clause_clean_glue") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "clause_clean_activity") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "glue_put_lev0_if_below_or_eq") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "glue_put_lev1_if_below_or_eq") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "every_lev1_reduce") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "every_lev2_reduce") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "do_bva") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "max_temp_lev2_learnt_clauses") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "never_stop_search") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "doMinimRedMoreMore") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "max_num_lits_more_more_red_min") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "max_glue_more_minim") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "orig_global_timeout_multiplier") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "num_conflicts_of_search_inc") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "more_red_minim_limit_binary") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "restart_inc") == 0) {
        return IPASIR_E_OK;
    }
    else if (strcmp(name, "restart_first") == 0) {
        return IPASIR_E_OK;
    }
    else {
        return IPASIR_E_OPTION_UNKNOWN;
    }
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
    static char tmp[200];
    std::string tmp2 = "cryptominisat-";
    tmp2 += SATSolver::get_version();
    memcpy(tmp, tmp2.c_str(), tmp2.length()+1);
    result = tmp;
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
    *result = (void*)new MySolver;
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
    MySolver* s = (MySolver*)solver;
    delete s;
    return IPASIR_E_OK;
}

namespace
{
void ensure_var_created(MySolver& s, Lit lit)
{
    if (lit.var() >= s.solver->nVars()) {
        const uint32_t toadd = lit.var() - s.solver->nVars() + 1;
        s.solver->new_vars(toadd);
    }
}
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
    MySolver* s = (MySolver*)solver;

    if (lit_or_zero == 0) {
        s->solver->add_clause(s->clause);
        s->clause.clear();
    } else {
        Lit lit(std::abs(lit_or_zero)-1, lit_or_zero < 0);
        ensure_var_created(*s, lit);
        s->clause.push_back(lit);
    }
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
    MySolver* s = (MySolver*)solver;
    Lit lit_cms(std::abs(lit)-1, lit < 0);
    ensure_var_created(*s, lit_cms);
    s->assumptions.push_back(lit_cms);
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
    MySolver* s = (MySolver*)solver;

    //Cleanup last_conflict
    for(auto x: s->last_conflict) {
        s->conflict_cl_map[x.toInt()] = 0;
    }
    s->last_conflict.clear();

    //solve
    lbool ret = s->solver->solve(&(s->assumptions));
    s->assumptions.clear();

    if (ret == l_True) {
        *result = 10;
        return IPASIR_E_OK;
    }
    if (ret == l_False) {
        s->conflict_cl_map.resize(s->solver->nVars()*2, 0);
        s->last_conflict = s->solver->get_conflict();
        for(auto x: s->last_conflict) {
            s->conflict_cl_map[x.toInt()] = 1;
        }
        *result = 20;
        return IPASIR_E_OK;
    }
    if (ret == l_Undef) {
        *result = 0;
        return IPASIR_E_OK;
    }
    return IPASIR_E_UNKNOWN;
}

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

IPASIR_API ipasir2_errorcode ipasir2_assignment_size(void* solver, int32_t* result) {
    return IPASIR_E_UNSUPPORTED;
}

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

IPASIR_API ipasir2_errorcode ipasir2_assignment(void* solver, int32_t index, int32_t* result) {
    return IPASIR_E_UNSUPPORTED;
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
     MySolver* s = (MySolver*)solver;
    assert(s->solver->okay());

    const int ipasirVar = std::abs(lit);
    const uint32_t cmVar = ipasirVar-1;
    lbool val = s->solver->get_model()[cmVar];

    if (val == l_Undef) {
        *result = 0;
    } else if (val == l_False) {
        *result = -ipasirVar;
    } else {
        *result = ipasirVar;
    }
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
    MySolver* s = (MySolver*)solver;
    const Lit tofind(std::abs(lit)-1, lit < 0);
    *result = s->conflict_cl_map[(~tofind).toInt()];
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
    return IPASIR_E_UNSUPPORTED;
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
    return IPASIR_E_UNSUPPORTED;
}

