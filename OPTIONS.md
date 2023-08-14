


/** 
 * @brief Specification of options for the configuration interface
 * 

 * 
 * To cover setting of limits:
 * - ipasir.limits.conflicts: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no conflict limit
 *    - 0: no conflicts (exit on first conflict)
 *    - n: at most n conflicts
 * - ipasir.limits.decisions: int, minimum: -1, maximum: INT_MAX, default: -1
 *    - -1: no decision limit
 *    - 0: no decisions (only unit propagation)
 *    - n: at most n decisions
 * - ...
 * 
 * To cover phase setting:
 * - ipasir.phase.initial: set the initial phase of the variables. 
 *      this is an indexed option. 
 *      use index=0 to set the phase of all variables.
 *      use index=i to set the phase of variable i.
 *   - -1: set all variables to false
 *   - 1: set all variables to true
 *   - 0: use the default phase initialization of the solver
 * 
 * To cover enabling/disabling preprocessing:
 * - ipasir.preprocessing: ...
 * 
 * 
 */