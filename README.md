# IPASIR-2 Draft and Proposal

The idea of creating an improved successor to the IPASIR interface and reviving the incremental library track of the SAT competition is now two years old. The initial impetus came in late 2022, when I began systematically collecting, grouping, and evaluating requests for improvements to IPASIR which have been floating around in the IPASIR Issue Tracker on GitHub, in my Mailbox, and in discussions with users and developers of SAT solvers. 
Not all of these ideas made it into the final draft of IPASIR-2, but here is a list of all the requests that were considered: [IDEAS.md](IDEAS.md).

We are pleased to present a draft that fulfills most of the above wishes and have created this GitHub area to share this preview of IPASIR-2 with a wider audience. In the [discussion area of this repository](https://github.com/orgs/ipasir2/discussions), you can share your constructive feedback on this draft, and notify us if you are missing a particular feature, or if you want to learn more about the background of a particular design decision.


## Error Codes

To improve the quality and stability of applications using IPASIR solvers in the backend, IPASIR-2 introduced error codes. All functions now return an error code and the actual data is returned via output parameters. With the error codes, IPASIR-2 introduces a programmatic way for solvers to inform applications about things that went wrong when calling an IPASIR function. Two of the most well-known use cases are covered by IPASIR2_E_UNSUPPORTED and IPASIR2_E_INVALID_STATE, to give examples. 

Solver authors who wish to implement only the core set of IPASIR functions, or who find it too complicated to implement some of the extended IPASIR functions, can now use a stub implementation that returns IPASIR2_E_UNSUPPORTED to elegantly notify applications of the lack of an implementation. Another common use case is covered by IPASIR2_E_INVALID_STATE, which is returned whenever a function is not applicable in the current state of the IPASIR state machine, for example when trying to access a model in UNSAT state.

With the introdution of error codes IPASIR-2 gives up backwards compatibility. This means the header name changed to ipasir2.h, the function names are now prefixed with ipasir2_, and wrappers are provided for both ipasir1 applications ans ipasir1 solvers.


## Configuration Interface

Most functions for use cases that were not covered by IPASIR 1 can now be covered with the new configuration interface. The proposed design considers extensibility, automatability as well as minimality. The configuration interface is extensible and generic, because most options are not applicable to all incremental solver implementations, and authors might want to expose their own proprietary configuration options. The interface is automatable for supporting automatic tuners, giving them a programmatic way to determine the available, tunable options. Moreover, the number of standardized options is kept minimal, and the only rule is that if the solver supports such a standardized option, it must be available under the standardized name. The configuration interface comes with two unique error codes IPASIR2_E_OPTION_UNKNOWN and IPASIR2_E_OPTION_INVALID_VALUE. 

Example use cases:

 - Controlling overhead in solving simple problems by disabling/enabling certain pre- and in-processing algorithms.
 - Search diversification for parallel portfolios
 - Setting hints about which variables are activation literals / will be assumptions and which will not
 - Freeze variables to prevent a solver from eliminating them
 - Setting the polarity of initial variables, for example to activate zero-first branching, random initialization of phases, or individual setting of phases
 - Return list of tunable heuristic parameters with min/max info for automatic tuners
 - Allow to set some options from an IPASIR callback, i.e., in SOLVING state

A use case driven documentation of options can be found in [OPTIONS.md](OPTIONS.md). 


## Clause Import 

There are several applications where SAT solvers need to be able to import clauses while in their SOLVING state. In parallel clause sharing portfolios, clauses which are exported by other solvers are asyncronously made available for import. In SAT modulo theories (SMT), clauses can be generated lazily by an external theory solver (cf. IPASIR-UP). To cover both use cases, IPASIR-2 offers the possibility to register a function for importing a clause from an external source, such that the solver can control when and how to import clauses. In case of parallel frameworks, it is up to the application to take care of concurrency issues while making clauses available to the import function. 

Depending on the use case, different redundancy notions might hold. In parallel clause sharing frameworks, learned clauses might preserve equivalence or, for example in case of extended resolution, at least preserve satisfiability. That does not necessarily hold for applications which lazily encode constraints, e.g., from a background theory. Most solvers should be able to import model-preserving redundant clauses but might not easily be enabled to import the latter.

To facilitate support for different use cases, there is a parameter over which a pledge can be set about what kind of importing clauses the solver must expect. These include equivalence and satisfiability preserving pledges, the latter allowing the import of blocked clauses and blocked sets, and the emergence of new variables, in particular. Applications that use the callback to add irredundant clauses (e.g. lazy encodings) can do so by setting pledges to none. Solvers can reject pledge levels that are too weak by signalling IPASIR2_E_UNSUPPORTED_ARGUMENT.

Imported irredundant clauses must be treated as original problem clauses and must not be forgotten. Note that in the case of IPASIR-UP, clauses resulting from lazy encodings can be safely imported as redundant due to the tight integration of user propagators.


## Assignment Notification

IPASIR-2 supports setting a callback function that exports recent changes to the current partial assignment in the form of two arrays, one for recently assigned variables and one for recently unassigned variables. One use case for this is to be notified about variables which are assigned when the decision limit is set to zero (cf. configuration interface). Applications which manage the interplay between several solvers can observe partial assignments for reinforcing diversification (in case of parallel search) or for deciding about which clauses to make available to the import clause callback (in case of parallel search or in case of an externally managed, lazily encoded background theory). 


