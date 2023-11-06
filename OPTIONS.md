# IPASIR-2 Options Specification Draft

In IPASIR-2, options are identified by a string. IPASIR-2 recommends using dot-separated namespaces for structuring them. The namespace "ipasir." is reserved for options specified in this document. The rule is to name the options as specified below if the option is supported. IPASIR-2 recommends supporting all the options specified below if possible. Other options can be propriatary or named by other conventions.

Solvers specify the supported options in an array of `ipasir2_option` structs which can be retrieved by calling **ipasir2_options()**.

```
typedef struct ipasir2_option {
    char const* name;

    int64_t min;
    int64_t max;

    ipasir2_state max_state;
    bool tunable;
    bool indexed;

    void* handle;
} ipasir2_option;
```

Options are set as **int64_t values** to simplify the *option setter* and the specification of *min and max values*. Solver authors need to map those in their wrapper code if they internally use floats. Simple base-10 fixed point arithmetic would probably solve most of those cases.

IPASIR-2 introduces the new state **CONFIG** which precedes input and can never be reached again during the lifetime of a solver. Moreover, IPASIR-2 specifies a partial order for its states: **CONFIG < INPUT = SAT = UNSAT < SOLVING**. This allows solver authors to specify the maximal state in which they can allow an option to be set. There might be options which have to be set before the solver object is created. In such a case the option would be marked with **max_state=CONFIG**. Options which can be reset between **solve()** calls would be marked with **max_state=INPUT**. And finally, for options which can be set from inside of one of the registered callback functions (use-case: IPASIR-UP) **max_state** would be **SOLVING**.

Not all options are eligible for tuning by an automated configuration tuning algorithm. In order to separate tunable from non-tunable options, the flag **tunable** exists. 
Some options can be set per variable or other types of indices. For this use case, the *option setter* has the additional parameter **index**. To separate options which can be set per index from those which can only be set globally, the flag **indexed** exists.


## Setting of limits
These options are usually not eligible for tuning. 
Use cases for setting deterministic solve limits include for example: 
- "Local Search with a SAT Oracle for Combinatorial Optimization" by Aviad Cohen, Alexander Nadel and Vadim Ryvchin (TACAS 2021)
- determination of implied facts by setting decision level to zero and listening to assignments with an ipasir2 notify callback

> `ipasir.limits.conflicts = n`
>  - `n = -1` no conflict limit
>  - otherwise exit when number of conflicts reaches n+1
>

> `ipasir.limits.decisions = n`
>  - `n = -1` no decision limit
>  - otherwise exit when number of decisions reaches n+1
>


## Activation of logging

Determination of the level of detail of logging.

> `ipasir.logging = n`
> - `n = 0` deactivate low-level logging
> - otherwise activate low-level logging
>


## Options which can be set for each variable

Use parameter index in setter to indicate the variable id, or zero if it shold be set for all variables.
### Initializing phases

Use cases for setting initial phases include activation of classic zero-first branching, usage of application specific phase initialization heuristics, or search diversification in parallel portfolios.

> `ipasir.phase.initial = n` 
> - `n = -1` set initial phase to false
> - `n = 1` set initial phase to true
> - `n = 0` use default
>

### Initializing branching order for solver using VSIDS

Use cases for setting initial branching order includes the setting of application specific branching priorities. 

> `ipasir.vsids.initial = n` 
> - set the initial vsids score to n
>

### Frozen variables

Many pre- and in-processing SAT solvers use elimination techinques such as bounded variable elimination (BVE) to simplify an instance. If an instance is satisfiable, eliminated variables have to be restored after search in order to determine a correct model. In incremental SAT solving, eliminated variables also have to be restored when they are used in assumptions or when they appear in imported clauses.

In some applications it is forseable which variables can occur in assumptions or imported clauses and which can not. As a performance optimization, such applications can set variables to a frozen state to entirely prevent the solver from eliminating them, thus preventing forseable on-demand restoring of clauses from the elimination stack. Moreover, if it is clear that a variable will never be used as an assumption (again), such applications can disable the frozen state for that variable.

> `ipasir.frozen = n`
> - `n=1` enable frozen state
> - `n=0` disable frozen state
>


## Non-incremental solving mode

This option introduces a one-shot solving mode, as requested in the ipasir issue tracker:
- https://github.com/biotomas/ipasir/pull/5

In one-shot solving the solver can throw any pre- and inprocessing technique at the instance, regardless of whether the solver is still usable after solving or not. 
If the option is activated, only _one more_ call to ipasir2_solve() is possible. All further calls to solve return an error code.

> `ipasir.yolo = n`
> - `n=1` non-incremental mode
> - `n=0` default incremental mode


## Assumption Handling

Specify how to propagate assumptions. Use cases include:
- "Speeding Up Assumption-Based SAT" by Randy Hickey and Fahiem Bacchus (SAT 2019)

> `ipasir.assumptions.propagate = n`
> - `n=0` propagate one assumption per decision level
> - `n=1` propagate all assumptions _at once_ on first decision level


## Preprocessing Options

Preprocessing configuration options.

> `ipasir.preprocessing = n`
> - `n=0` disable
> - `n=1` enable

### Variable Elimination Options

> `ipasir.preprocessing.elim = n`
> - `n=0` disable
> - `n=1` enable

### Subsumption Options

> `ipasir.preprocessing.subsumption = n`
> - `n=0` disable
> - `n=1` enable






