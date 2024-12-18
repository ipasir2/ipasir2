# IPASIR-2 Options Specification Draft

Each IPASIR-2 solver provides an array of supported options which can be obtained by calling **ipasir2_options()**.
Each option is defined by the struct `ipasir2_option` the fields of which are described below.

## The `ipasir2_option` struct

```c
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

### The `name` field

In IPASIR-2, options are identified by a string using dot-separated namespaces for structuring.
The top-level namespace "ipasir" is reserved for the options specified in the IPASIR-2 standard.
If an IPASIR-2 solver supports a standard option, it is guaranteed to have the name specified in the standard.
It is recommended, but not mandatory, that IPASIR-2 solvers support all standard options.
Other proprietary options may be specified in the solver documentation, but must not be in the "ipasir" namespace.

### The `min` and `max` fields

The `min` and `max` fields specify the range of values that the option can take.
To simplify the interface, all option values are given as 64-bit integers.
Solvers that use floats internally must map them in their wrapper code.

### The `max_state` field

The `max_state` field specifies the maximal solver state in which the option can be set.
While some options can be set at any time, for example, from a callback during solving, others can only be set before the first call to `ipasir2_solve()`, or only between calls to `ipasir2_solve()`.
For these use cases, IPASIR-2 introduces the new state **CONFIG**, which precedes input and can never be reached again during the lifetime of a solver.
IPASIR-2 specifies a partial order for its states: **CONFIG < INPUT = SAT = UNSAT < SOLVING**.
This allows solver authors to specify the maximum state in which they will allow an option to be set.

### The `tunable` field

The `tunable` field specifies whether the option is eligible for tuning by an automated configuration tuning algorithm, since not all options are suitable for tuning.

### The `indexed` field

Many options are global and can only be set once for the solver.
However, some options can be set per variable or other types of indices.
The `indexed` field specifies whether the option can be set per variable or other types of indices.

### The `handle` field

The `handle` field is used by the option setter to identify the option.


## Standard Options

The standard options can roughly be divided into tunable options and non-tunable options.
The following sections describe the standard options in detail.

### Non-tunable Options

Non-tunable options include the setting of limits, the setting of the non-incremental solving mode, and the setting of variable specific options.

#### Setting of limits

> `ipasir.limits.conflicts = n`
>  - `n = -1` no conflict limit (default)
>  - otherwise exit when number of conflicts reaches n+1

> `ipasir.limits.decisions = n`
>  - `n = -1` no decision limit (default)
>  - otherwise exit when number of decisions reaches n+1

Use cases for setting deterministic solve limits include for example: 
- "Local Search with a SAT Oracle for Combinatorial Optimization" by Aviad Cohen, Alexander Nadel and Vadim Ryvchin (TACAS 2021)
- determination of implied facts by setting decision level to zero and listening to assignments with an ipasir2 fixed callback

#### Non-incremental solving mode

> `ipasir.yolo = n`
> - `n=0` incremental mode (default)
> - `n=1` non-incremental / one-shot solving / yolo mode

This option introduces a one-shot solving mode, as requested in the ipasir issue tracker:
https://github.com/biotomas/ipasir/pull/5

In one-shot solving the solver can throw any pre- and inprocessing technique at the instance, regardless of whether the solver is still usable after solving or not. 
If the option is activated, only _one more_ call to ipasir2_solve() is possible. All further calls to solve return an error code.

#### Options which can be set for each variable

Use parameter index in setter to indicate the variable id, or zero if it shold be set for all variables.

##### Initializing phases

> `ipasir.variables.phase.initial = n` 
> - `n = 0` use default initial phase (default)
> - `n = -1` set initial phase to false
> - `n = 1` set initial phase to true

Use cases for setting initial phases include activation of classic zero-first branching, usage of application specific phase initialization heuristics, or search diversification in parallel portfolios.

##### Initializing branching order

> `ipasir.variables.score.initial = n` 
> - set the initial variable score to n

Use cases for setting initial branching order includes the setting of application specific branching priorities. 

##### Frozen variables

> `ipasir.variables.frozen = n`
> - `n=0` disable frozen state (default)
> - `n=1` enable frozen state

Many pre- and in-processing SAT solvers use elimination techinques such as bounded variable elimination (BVE) to simplify an instance.
If an instance is satisfiable, eliminated variables have to be restored after search in order to determine a correct model.
In incremental SAT solving, eliminated variables also have to be restored when they are used in assumptions or when they appear in imported clauses.

In some applications it is forseable which variables can occur in assumptions or imported clauses and which can not.
As a performance optimization, such applications can set variables to a frozen state to entirely prevent the solver from eliminating them, thus preventing forseable on-demand restoring of clauses from the elimination stack.
Moreover, if it is clear that a variable will never be used as an assumption (again), such applications can disable the frozen state for that variable.

#### Assumption Handling

##### Propagation of assumptions

> `ipasir.assumptions.propagate = n`
> - `n=0` propagate one assumption per decision level (default)
> - `n=1` propagate all assumptions _at once_ on 'first' decision level

Specify how to propagate assumptions. Use cases include:
- "Speeding Up Assumption-Based SAT" by Randy Hickey and Fahiem Bacchus (SAT 2019)

##### Treat assumptions as fixed

> `ipasir.assumptions.fixed = n`
> - `n=0` do not treat assumptions as fixed (default)
> - `n=1` treat assumptions as fixed

Normally the fixed() callback only notifies about fixed assignments at level zero. With this option enabled, use the fixed() callback to also notify about literals implied by assumptions.
