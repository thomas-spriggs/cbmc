// Author: Diffblue Ltd.

/// \file
/// Utilities for making a map of types to associated sizes.

#ifndef CPROVER_SOLVERS_SMT2_POINTER_SIZE_TRACKING_H
#define CPROVER_SOLVERS_SMT2_POINTER_SIZE_TRACKING_H

#include <util/expr.h>

#include <solvers/smt2_incremental/object_tracking.h>
#include <solvers/smt2_incremental/smt_object_size.h>
#include <solvers/smt2_incremental/smt_terms.h>

#include <unordered_map>

using pointer_size_mapt = std::unordered_map<typet, smt_termt, irep_full_hash>;

/// Establish pointer-sizes map for all pointers present in the
/// expression argument.
/// \param expression: the expression we're building the map for.
/// \param ns: a namespace - passed to size_of_expr to find expression sizes.
/// \param pointer_size_map: the map containing the pointer.base_type() -> size (in bytes) mappings.
/// \param object_map: passed through to convert_expr_to_smt
/// \param object_size: passed through to convert_expr_to_smt
void associate_pointer_sizes(
  const exprt &expression,
  const namespacet &ns,
  pointer_size_mapt &pointer_size_map,
  const smt_object_mapt &object_map,
  const smt_object_sizet::make_applicationt &object_size);

#endif
