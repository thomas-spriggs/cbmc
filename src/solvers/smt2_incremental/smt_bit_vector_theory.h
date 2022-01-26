// Author: Diffblue Ltd.

#ifndef CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_BIT_VECTOR_THEORY_H
#define CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_BIT_VECTOR_THEORY_H

#include <solvers/smt2_incremental/smt_terms.h>

class smt_bit_vector_theoryt
{
public:
#define SMT_BITVECTOR_THEORY_OPERATOR(the_identifier, the_name, the_return_sort)               \
  /* NOLINTNEXTLINE(readability/identifiers) cpplint does not match the ## */  \
  struct the_name##t final                                                     \
  {                                                                            \
    static const char *identifier();                                           \
    static smt_sortt                                                           \
    return_sort(const smt_termt &left, const smt_termt &right);                \
    static void validate(const smt_termt &left, const smt_termt &right);       \
  };                                                                           \
  static const smt_function_application_termt::factoryt<the_name##t> the_name;
#include "smt_bit_vector_theory.def"
#undef SMT_BITVECTOR_THEORY_PREDICATE
#undef SMT_BITVECTOR_THEORY_OPERATOR
};

#endif // CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_BIT_VECTOR_THEORY_H
