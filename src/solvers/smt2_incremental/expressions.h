// Author: Diffblue Ltd.

/// \file
/// Expressions for use in incremental SMT2 decision procedure

#ifndef CBMC_EXPRESSIONS_H
#define CBMC_EXPRESSIONS_H

#include <util/expr.h>

const irep_idt ID_nondet_padding = "nondet_padding";

class nondet_padding_exprt : public expr_protectedt
{
public:
  explicit nondet_padding_exprt(typet type) :
      expr_protectedt{ID_nondet_padding, std::move(type)}
      {}
};

template <>
inline bool can_cast_expr<nondet_padding_exprt>(const exprt &base)
{
  return base.id() == ID_nondet_padding;
}

#endif // CBMC_EXPRESSIONS_H
