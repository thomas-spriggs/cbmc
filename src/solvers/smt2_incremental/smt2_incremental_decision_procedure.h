// Author: Diffblue Ltd.

/// \file
/// Decision procedure with incremental SMT2 solving.

#ifndef CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H
#define CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H

#include <solvers/smt2_incremental/smt_terms.h>
#include <util/expr.h>
#include <solvers/stack_decision_procedure.h>
#include <util/piped_process.h>

    	#include <iostream>
    	#define WATCHVAR( var ) \
    	  std::cerr << "DBG: " << __FILE__ << "(" << __LINE__ << ") " << #var << \
    	    " = [" << (var) << "]" << std::endl

class smt2_incremental_decision_proceduret final
  : public stack_decision_proceduret
{
public:
  /// \param solver_command: The command and arguments for invoking the smt2
  ///                        solver.
  explicit smt2_incremental_decision_proceduret(std::string solver_command);

  // Implementation of public decision_proceduret member functions.
  exprt handle(const exprt &expr) override;
  exprt get(const exprt &expr) const override;
  void print_assignment(std::ostream &out) const override;
  std::string decision_procedure_text() const override;
  std::size_t get_number_of_solver_calls() const override;
  void set_to(const exprt &expr, bool value) override;

  // Implementation of public stack_decision_proceduret member functions.
  void push(const std::vector<exprt> &assumptions) override;
  void push() override;
  void pop() override;

protected:
  // Implementation of protected decision_proceduret member function.
  resultt dec_solve() override;

  template <typename smt_typet>
  void send_to_solver(const smt_typet &smt)
  {
    const auto solver_text = smt_to_smt2_string(smt);
    WATCHVAR(solver_text);
    solver_process.send(solver_text);
  }

  /// This is where we store the solver command for reporting the solver used.
  std::string member_solver_command;
  size_t number_of_solver_calls;

  piped_processt solver_process;

  class sequencet {
    size_t last_id = 0;
  public:
    size_t operator()()
    {
      return ++last_id;
    }
  } sequence;

  std::unordered_map<exprt, smt_identifier_termt, irep_hash>
    defined_expressions;

  /// \brief Defines a new function to represent \p expr if it doesn't exisit
  ///   already.
  /// \returns the identifier of either the new or existing function.
  const smt_identifier_termt &define_function(const exprt &expr);
};

#endif // CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H
