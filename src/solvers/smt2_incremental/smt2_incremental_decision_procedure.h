// Author: Diffblue Ltd.

/// \file
/// Decision procedure with incremental SMT2 solving.

#ifndef CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H
#define CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H

#include <solvers/stack_decision_procedure.h>
#include <util/piped_process.h>

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
    solver_process.send(smt_to_smt2_string(smt));
  }

  /// This is where we store the solver command for reporting the solver used.
  std::string member_solver_command;
  size_t number_of_solver_calls;

  piped_processt solver_process;
};

#endif // CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT2_INCREMENTAL_DECISION_PROCEDURE_H
