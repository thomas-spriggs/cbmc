// Author: Diffblue Ltd.

#include "smt2_incremental_decision_procedure.h"
#include "convert_expr_to_smt.h"

#include <solvers/smt2_incremental/smt_commands.h>
#include <solvers/smt2_incremental/smt_to_smt2_string.h>
#include <solvers/smt2_incremental/smt_terms.h>
#include <solvers/smt2_incremental/smt_core_theory.h>
#include <util/expr.h>
#include <util/expr_cast.h>
#include <util/std_expr.h>
#include <util/string_utils.h>

/// Find all sub expressions of the given \p expr which need to be expressed
/// as separate smt commands.
static std::vector<exprt> gather_dependent_expressions(const exprt &expr)
{
  std::vector<exprt> dependent_expressions;
  expr.visit_pre([&](const exprt &expr_node)
  {
     if(can_cast_expr<symbol_exprt>(expr_node))
     {
       dependent_expressions.push_back(expr_node);
     }
  });
  return dependent_expressions;
}

    	#include <iostream>
    	#define WATCHVAR( var ) \
    	  std::cerr << "DBG: " << __FILE__ << "(" << __LINE__ << ") " << #var << \
    	    " = [" << (var) << "]" << std::endl


smt2_incremental_decision_proceduret::smt2_incremental_decision_proceduret(
  std::string solver_command)
  : member_solver_command{std::move(solver_command)},
    number_of_solver_calls{0},
    solver_process{split_string(member_solver_command, ' ', false, true)}
{
  send_to_solver(smt_set_option_commandt{smt_option_produce_modelst{true}});
  send_to_solver(
    smt_set_logic_commandt{
      smt_logic_quantifier_free_uninterpreted_functions_bit_vectorst{}});
}

exprt smt2_incremental_decision_proceduret::handle(const exprt &expr)
{
  return expr;
}

exprt smt2_incremental_decision_proceduret::get(const exprt &expr) const
{
  UNIMPLEMENTED_FEATURE("`get` of:\n  " + expr.pretty(2, 0));
}

void smt2_incremental_decision_proceduret::print_assignment(
  std::ostream &out) const
{
  UNIMPLEMENTED_FEATURE("printing of assignments.");
}

std::string
smt2_incremental_decision_proceduret::decision_procedure_text() const
{
  return "incremental SMT2 solving via \"" + member_solver_command + "\"";
}

std::size_t
smt2_incremental_decision_proceduret::get_number_of_solver_calls() const
{
  return number_of_solver_calls;
}

void smt2_incremental_decision_proceduret::set_to(const exprt &expr, bool value)
{
  PRECONDITION(can_cast_type<bool_typet>(expr.type()));
  std::cerr << "`set_to` (" + std::string{value ? "true" : "false"} + "):\n  " +
    expr.pretty(2, 0) << std::endl;

  auto converted_term = convert_expr_to_smt(expr);
  if(!value)
    converted_term = smt_core_theoryt::make_not(converted_term);
  send_to_solver(smt_assert_commandt{converted_term});

  //UNIMPLEMENTED_FEATURE(
  //  "`set_to` (" + std::string{value ? "true" : "false"} + "):\n  " +
  //  expr.pretty(2, 0));
}

void smt2_incremental_decision_proceduret::push(
  const std::vector<exprt> &assumptions)
{
  for(const auto &assumption : assumptions)
  {
    UNIMPLEMENTED_FEATURE(
      "pushing of assumption:\n  " + assumption.pretty(2, 0));
  }
  UNIMPLEMENTED_FEATURE("`push` of empty assumptions.");
}

void smt2_incremental_decision_proceduret::push()
{
  UNIMPLEMENTED_FEATURE("`push`.");
}

void smt2_incremental_decision_proceduret::pop()
{
  UNIMPLEMENTED_FEATURE("`pop`.");
}

decision_proceduret::resultt smt2_incremental_decision_proceduret::dec_solve()
{
  ++number_of_solver_calls;
  UNIMPLEMENTED_FEATURE("solving.");
}

const smt_identifier_termt &
smt2_incremental_decision_proceduret::define_function(const exprt &expr)
{
  const auto existing_definition = defined_expressions.find(expr);
  if(existing_definition != defined_expressions.cend())
  {
    return existing_definition->second;
  }

  smt_define_function_commandt function{
    "B" + std::to_string(sequence()),
    {},
    convert_expr_to_smt(expr)};
  defined_expressions.emplace(expr, function.identifier());
  send_to_solver(function);
  return function.identifier();
}
