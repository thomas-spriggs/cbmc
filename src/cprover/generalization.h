/*******************************************************************\

Module: Generalization

Author: Daniel Kroening, dkr@amazon.com

\*******************************************************************/

/// \file
/// Generalization

#ifndef CPROVER_CPROVER_GENERALIZATION_H
#define CPROVER_CPROVER_GENERALIZATION_H

#include "solver_types.h" // IWYU pragma: keep

class chc_solver_optionst;

void generalization(
  std::vector<framet> &frames,
  const workt &dropped,
  const propertyt &,
  const chc_solver_optionst &);

#endif // CPROVER_CPROVER_GENERALIZATION_H
