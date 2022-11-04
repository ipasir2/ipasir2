#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>  
#include <sys/resource.h> 

// PBLib
// #include "incpbconstraint.h"
// #include "PBConfig.h"
#include "PBParser.h"
#include "pb2cnf.h"
#include "VectorClauseDatabase.h"
#include "SATSolverClauseDatabase.h"
#include <sys/time.h>  
// MiniSAT
#include "BasicSATSolver.h"


using namespace PBLib;
using namespace std;

bool check_constraints(vector<int32_t> const & model, vector<PBConstraint> const & constraints)
{
  bool ok = true;
  for (PBConstraint const & constraint : constraints)
  {
    constraint.print();
    int64_t sum = 0;
    
    for (WeightedLit lit : constraint.getWeightedLiterals())
    {
      if (model[abs(lit.lit)] == lit.lit)
	sum += lit.weight;
    }
    
    cout << (sum  >= constraint.getGeq() ? "OK" : "FALSE") << " sum = " << sum << endl;
  }
  
  return ok;
}

template <class P>
int64_t getSumFromModel(P & constraint, BasicSATSolver * satsolver)
{  
    vector<int32_t> model;
    satsolver->getModel(model);
    
    int64_t sum = 0;
    
    for (int i = 0; i < constraint.getN(); ++i)
    {
      if (model[abs(constraint.getWeightedLiterals()[i].lit)] * (constraint.getWeightedLiterals()[i].lit > 0 ? 1 : -1) > 0)
	sum += constraint.getWeightedLiterals()[i].weight;
    }
    
    return sum;
}




void basicSearch(bool cnf_output_only, double tstart, vector<PBConstraint> & constraints, PBParser & parser, PBConfig config)
{
  statistic stats;
  PB2CNF pb2cnf(config, &stats);
  
  double tend;
  VectorClauseDatabase formula(config);
  
  BasicSATSolver * satsolver = new BasicSATSolver();
  satsolver->increseVariables(parser.getMaxVarID());
  
  
  cout << "c start encoding ... "; cout.flush();
  if (config->print_used_encodings)
    cout << endl;
  
  AuxVarManager initAuxVars(parser.getMaxVarID() + 1);
  
  
  for(int i = 0; i < (int) constraints.size(); ++i)
  {
      pb2cnf.encode(constraints[i], formula, initAuxVars);
  }
  
  tend = clock();
  cout << "done (parsing and encoding: " << (tend - tstart) / CLOCKS_PER_SEC << " sec )" << endl;
  
  
  tstart = tend;

  
  for (auto clause : formula.getClauses())
    satsolver->addClause(clause);
  
  bool result = satsolver->solve();
  
  if(!result)
  {
    cout << "s UNSATISFIABLE" << endl;
    delete satsolver;
    return;
  }
  else if (!parser.hasObjectiveFunction())
  {
    
    cout << "s SATISFIABLE" << endl;
    vector<int32_t> model;

     if (config->cmd_line_options.find("model") != config->cmd_line_options.end())
     {
      satsolver->printModel();
     }
     delete satsolver;
     return;
  }
  
 
  PBConstraint opt_constraint = parser.getObjConstraint();

  if (opt_constraint.getComparator() == BOTH)
  {
    // TODO this should be very simple
    cout << "c opt constraint with LEQ and GEQ is not supported yet" << endl;
  }
  
  assert(opt_constraint.getComparator() == LEQ);
  
  int64_t current_bound = getSumFromModel(opt_constraint, satsolver);;
    
  
  int32_t first_free_var = initAuxVars.getBiggestReturnedAuxVar() + 1;
  
  while (true)
  {
    cout << "o " << current_bound << endl;
    opt_constraint.setLeq(current_bound - 1);

    AuxVarManager opt_aux(first_free_var);
    
    delete satsolver;
    satsolver = new BasicSATSolver();
    satsolver->increseVariables(parser.getMaxVarID());
    
    VectorClauseDatabase opt_formula(config);
    
    for (auto clause : formula.getClauses())
	satsolver->addClause(clause);  
    
    pb2cnf.encode(opt_constraint, opt_formula, opt_aux);
    satsolver->addClauses(opt_formula.getClauses());
    
    
    if (!satsolver->solve())
      break;
    
    current_bound = getSumFromModel(opt_constraint, satsolver);
  }

  cout << "s OPTIMUM FOUND" << endl;
  delete satsolver;
}


void analyse(char** argv)
{
  PBConfig config = make_shared<PBConfigClass>();
  statistic stats;
  PB2CNF pb2cnf(config, &stats);

  PreEncoder  pre_encoder(config);
        
  PBParser parser;

//   cout << "c start parsing ... "; cout.flush();
  
  vector<PBConstraint> constraints = parser.parseFile(argv[1]);
  
  if (!parser.isOk())
  {
    cout << "c error: could not parse input file" << endl;
    cout << "0" << endl;
    exit(-1);
  }
  
//   cout << "done" << endl;
  
  
  AuxVarManager auxVars(parser.getMaxVarID() + 1);
  
  CountingClauseDatabase formula(config);
  
  int64_t count = 0;
  
  for(auto c : constraints)
  {
    SimplePBConstraint s = pre_encoder.preEncodePBConstraint(c, formula);
    if (s.getType() == PB)
      count++;
  }
  
  cout << count << endl;
}


int main(int argc, char **argv)
{
  
  double tstart = clock(), tend;
  
  bool  cnf_output_only = false;

  int cpu_lim = -1; // Limit on CPU time allowed in seconds
  int mem_lim = -1; // Limit on memory usage in megabytes
  
  // Set limit on CPU-time: (copy and paste from glucose source code)
  if (cpu_lim != -1){
      rlimit rl;
      getrlimit(RLIMIT_CPU, &rl);
      if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max){
	  rl.rlim_cur = cpu_lim;
	  if (setrlimit(RLIMIT_CPU, &rl) == -1)
	      printf("c WARNING! Could not set resource limit: CPU-time.\n");
      } }

  // Set limit on virtual memory:
  if (mem_lim != -1){
      rlim_t new_mem_lim = (rlim_t)mem_lim * 1024*1024;
      rlimit rl;
      getrlimit(RLIMIT_AS, &rl);
      if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max){
	  rl.rlim_cur = new_mem_lim;
	  if (setrlimit(RLIMIT_AS, &rl) == -1)
	      printf("c WARNING! Could not set resource limit: Virtual memory.\n");
      } }
  // end of copy and paste
  
  if (argc < 2)
  {
    cout << "usage " << argv[0] << " inputfile [options]" << endl;
    return -1;
  }
  
  PBConfig config = make_shared<PBConfigClass>();
//     config->pb_encoder = PB_ENCODER::SORTINGNETWORKS;
//   config->pb_encoder = PB_ENCODER::ADDER;
//   config->pb_encoder = PB_ENCODER::BINARY_MERGE;
//   config->print_used_encodings = true;
  config->MAX_CLAUSES_PER_CONSTRAINT = 1000000;  
//   config->amo_encoder = AMO_ENCODER::COMMANDER;    
//   config->use_formula_cache=false;
//   config->check_for_dup_literals = true;
  config->print_used_encodings = false;
//   config->pb_encoder = PB_ENCODER::ADDER;
  
  
  //TODO refactor this
  unordered_map<string,string> options;
  
  if (argc > 2)
  {
    string tmp;
    for (int i = 2; i < argc; ++i)
    {
      tmp = string(argv[i]);
      while (tmp[0] == '-')
	tmp = tmp.substr(1);
      config->cmd_line_options.insert(tmp);
    }
    

    for (int i = 2; i < argc; ++i)
    {
	tmp = string(argv[i]);
	while (tmp[0] == '-')
	    tmp = tmp.substr(1);	

	options[tmp.substr(0,tmp.find("="))] = tmp.find("=") == string::npos ? "" : tmp.substr(tmp.find("=") + 1);
    }
  }
  
  if (config->cmd_line_options.find("cnf") != config->cmd_line_options.end())
    cnf_output_only = true;
  
  if (config->cmd_line_options.find("norobdds") != config->cmd_line_options.end())
    config->use_real_robdds = false;
  
  if (config->cmd_line_options.find("rectest") != config->cmd_line_options.end())
    config->use_recursive_bdd_test = true;
  
  if (config->cmd_line_options.find("itertest") != config->cmd_line_options.end())
    config->use_recursive_bdd_test = false;
  
  
  if (config->cmd_line_options.find("nested") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::NESTED;
  
  if (config->cmd_line_options.find("amo_bdd") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::BDD;
  
  if (config->cmd_line_options.find("bimander") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::BIMANDER;
  
  if (config->cmd_line_options.find("commander") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::COMMANDER;
  
  if (config->cmd_line_options.find("kproduct") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::KPRODUCT;
  
  if (config->cmd_line_options.find("binary") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::BINARY;
  
  if (config->cmd_line_options.find("pairwise") != config->cmd_line_options.end())
    config->amo_encoder = AMO_ENCODER::PAIRWISE;
  
  if (config->cmd_line_options.find("amk_bdd") != config->cmd_line_options.end())
    config->amk_encoder = AMK_ENCODER::BDD;
  
  if (config->cmd_line_options.find("card") != config->cmd_line_options.end())
    config->amk_encoder = AMK_ENCODER::CARD;
  
  if (config->cmd_line_options.find("pb_bdd") != config->cmd_line_options.end())
    config->pb_encoder = PB_ENCODER::BDD;

  if (config->cmd_line_options.find("pb_adder") != config->cmd_line_options.end())
    config->pb_encoder = PB_ENCODER::ADDER;

  if (config->cmd_line_options.find("pb_sorter") != config->cmd_line_options.end())
    config->pb_encoder = PB_ENCODER::SORTINGNETWORKS;
  
  if (config->cmd_line_options.find("watchdog") != config->cmd_line_options.end())
  {
    config->pb_encoder = PB_ENCODER::BINARY_MERGE;
    config->use_watch_dog_encoding_in_binary_merger = true;
  }
  
  
  if (config->cmd_line_options.find("bin_merge") != config->cmd_line_options.end())
    config->pb_encoder = PB_ENCODER::BINARY_MERGE;
  
  if (config->cmd_line_options.find("bin_merge_no_gac") != config->cmd_line_options.end())
    config->use_gac_binary_merge = false;
  
  if (config->cmd_line_options.find("watchdog_no_gac") != config->cmd_line_options.end())
    config->use_gac_binary_merge = false;
  
  if (config->cmd_line_options.find("bin_merge_all_support") != config->cmd_line_options.end())
    config->binary_merge_no_support_for_single_bits = false;
 
  
  if (config->cmd_line_options.find("non_gac_bdds") != config->cmd_line_options.end())
    config->debug_value = "non_gac_bdds";
  
  if (config->cmd_line_options.find("test") != config->cmd_line_options.end())
    config->debug_value = "test";
  
  if (config->cmd_line_options.find("analyse") != config->cmd_line_options.end())
  {
    analyse(argv);
    return 0;
  }
  
  BasicSATSolver satsolver;
  SATSolverClauseDatabase formula(config, &satsolver);
  

      
  PBParser parser;

  cout << "c start parsing ... "; cout.flush();
  
  vector<PBConstraint> constraints = parser.parseFile(argv[1]);
  
  if (!parser.isOk())
  {
    cout << "c error: could not parse input file" << endl;
    exit(-1);
  }
  
  satsolver.increseVariables(parser.getMaxVarID());

  cout << "done" << endl;

  basicSearch(cnf_output_only, tstart, constraints, parser, config);
  
  
  tend = clock();
  cout << "wall time: " << (tend - tstart) / CLOCKS_PER_SEC << " sec" << endl;

  struct rusage rusage;
  getrusage( RUSAGE_SELF, &rusage );
  cout <<"Memory usage: "<< (size_t)(rusage.ru_maxrss / 1024L) << " mbyte" << endl; 
  
  return 0;
}
