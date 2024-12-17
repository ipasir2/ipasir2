# Welcome to the IPASIR-2 Workgroup!

This GitHub space exists for bringing together the users and developers of incremental SAT solvers for discussing the details of the next version of the _Re-entrant Incremental SAT solver API_  (IPASIR).
The reverse acronym IPASIR is pronounced as in "IPA, Sir?"
With this proposal we ambitiously and consequently "Make it two!"

IPASIR was originally specified in SAT Race 2015 [1].
Since then, IPASIR has been the standard interface for evaluating the performance of incremental SAT solvers in the annual international SAT competitions.
Many applications of incremental SAT solvers come with more than one interface to support using different SAT solvers in their backend.
Today, such applications can use IPASIR to support using all the SAT solvers implementing that interface.
The possibility to use a standard interface has many advantages for both application and SAT solver developers.


## Features of IPASIR-2

But in its current state, IPASIR has several limitations.
The community has raised a multitude of issues and missing features.
The largest number of such requests is related to the **configuration** of SAT solvers, i.e., users want to be able to activate or deactivate particular options in SAT solvers.
Use-cases of a SAT solver configuration interface encompass switching between light-weight and heavy-weight solving modes, search diversification in parallel portfolios, or solver tuning with automatic configuration tools, to name a few.

For applications of incremental SAT solvers, the standard, flexible and easy access to a portfolio of solvers is one of the traditional use-cases of IPASIR.
In the typical parallel portfolio framework, SAT solvers exchange information in terms of learned clauses.
Since IPASIR only supports the export of learned clauses, IPASIR-2 should also support the **asynchronous import of clauses**.

With the two new features, i.e. the **configuration interface** and the **asynchronous clause import**, tasks like solver tuning, diversification and information sharing can be implemented on top of IPASIR-2.
This has the advantage that new solvers can be easily integrated into existing solutions and frameworks.

Other proposed features include means for clean **error handling** with IPASIR error codes, and the notification about **variable assignments**.
In this workgroup, we are discussing and drafting all new features of IPASIR-2.
In particular, this is about the following new features.

### 1. Configuration Interface

### 2. Asynchronous Clause Import

### 3. Notification on Changes in Variable Assignments

### 4. Error Codes


## Language and Compatibility

IPASIR-2 is planned as a C interface, but providing suitable C++, Python, Rust and Java bindings is part of the plan.
Maintaining backward compatibility, i.e., all IPASIR-2 solvers are also IPASIR solvers, is very likely but is already being discussed.
To cover most new use cases, the configuration interface and the asynchronous clause import seem to be the most important new features of IPASIR-2.


## References

[1] Tomáŝ Balyo, Armin Biere, Markus Iser, and Carsten Sinz. SAT Race 2015. (Artif. Intell., 241:45–
65, 2016).
