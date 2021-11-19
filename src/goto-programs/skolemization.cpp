/// \file skolemization.h
/// Rename variables in existentially quantified statements.
/// Author: Diffblue Ltd.

#include "skolemization.h"

#include "goto_model.h"

#include <util/fresh_symbol.h>
#include <util/irep.h> // for id2string
#include <util/mathematical_expr.h>

/// Perform the first part of skolemisation, by identifying an existentially
/// quantified statement and substituting the variables with skolem constants.
/// \param condition The condition on which we search for quantifiers.
/// \param symbol_table The symbol_table new symbols are inserted into.
static void rename_variables_in_existential_quantifiers(
  exprt &condition,
  symbol_table_baset &symbol_table)
{
  condition.visit_post([&symbol_table](exprt &sub_expression) {
    if(can_cast_expr<exists_exprt>(sub_expression))
    {
      quantifier_exprt &quantifier_expr = to_quantifier_expr(sub_expression);
      const auto &quantifier_sym = quantifier_expr.symbol();

      const auto new_symbol = get_fresh_aux_symbol(
        quantifier_sym.type(),
        CPROVER_PREFIX,
        id2string(
          symbol_table.lookup(quantifier_sym.get_identifier())->base_name),
        quantifier_expr.source_location(),
        ID_C,
        symbol_table);
      const auto symbol_expr =
        symbol_exprt(new_symbol.name, quantifier_sym.type());
      const std::vector<exprt> new_sym_vec{symbol_expr};

      sub_expression =
        exists_exprt(symbol_expr, quantifier_expr.instantiate(new_sym_vec));
    }
  });
}

void rename_variables_in_existential_quantifiers(goto_modelt &goto_model)
{
  for(auto &function : goto_model.goto_functions.function_map)
  {
    for(auto &instruction : function.second.body.instructions)
    {
      if(instruction.has_condition())
      {
        rename_variables_in_existential_quantifiers(
          instruction.condition_nonconst(), goto_model.symbol_table);
      }
    }
  }
}
