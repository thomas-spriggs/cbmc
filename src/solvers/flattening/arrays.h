/*******************************************************************\

Module: Theory of Arrays with Extensionality

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Theory of Arrays with Extensionality

#ifndef CPROVER_SOLVERS_FLATTENING_ARRAYS_H
#define CPROVER_SOLVERS_FLATTENING_ARRAYS_H

#include <list>
#include <set>
#include <unordered_set>

#include <util/union_find.h>

#include "equality.h"

class array_of_exprt;
class equal_exprt;
class if_exprt;
class index_exprt;
class with_exprt;
class update_exprt;

class arrayst:public equalityt
{
public:
  arrayst(
    const namespacet &_ns,
    propt &_prop,
    message_handlert &message_handler);

  void post_process() override
  {
    post_process_arrays();
    SUB::post_process();
  }

  // NOLINTNEXTLINE(readability/identifiers)
  typedef equalityt SUB;

  literalt record_array_equality(const equal_exprt &expr);
  void record_array_index(const index_exprt &expr);

protected:
  const namespacet &ns;

  virtual void post_process_arrays()
  {
    add_array_constraints();
  }

  struct array_equalityt
  {
    literalt l;
    exprt f1, f2;
  };

  // The list of all equalities between arrays.
  // References to objects in `array_equalities` need to be stable because
  // elements are added while references are held.
  std::list<array_equalityt> array_equalities;

  // This is used to find the clusters of arrays being compared.
  union_find<exprt, irep_hash> arrays;

  // This tracks the array indices for each array.
  typedef std::set<exprt> index_sett;
  // References to values in `index_map` need to be stable because
  // elements are added while references are held.
  std::map<std::size_t, index_sett> index_map;

  enum class lazy_typet
  {
    ARRAY_ACKERMANN,
    ARRAY_WITH,
    ARRAY_IF,
    ARRAY_OF,
    ARRAY_TYPECAST,
    ARRAY_CONSTANT,
    ARRAY_COMPREHENSION
  };

  struct lazy_constraintt
  {
    lazy_typet type;
    exprt lazy;

    lazy_constraintt(lazy_typet _type, const exprt &_lazy)
    {
      type = _type;
      lazy = _lazy;
    }
  };

  bool lazy_arrays;
  bool incremental_cache;
  std::list<lazy_constraintt> lazy_array_constraints;
  // Adds array constraints lazily.
  void add_array_constraint(const lazy_constraintt &lazy, bool refine = true);
  std::map<exprt, bool> expr_map;

  // Adds all the constraints eagerly.
  void add_array_constraints();
  void add_array_Ackermann_constraints();
  void add_array_constraints_equality(
    const index_sett &index_set, const array_equalityt &array_equality);
  void add_array_constraints(
    const index_sett &index_set, const exprt &expr);
  void add_array_constraints_if(
    const index_sett &index_set, const if_exprt &exprt);
  void add_array_constraints_with(
    const index_sett &index_set, const with_exprt &expr);
  void add_array_constraints_update(
    const index_sett &index_set, const update_exprt &expr);
  void add_array_constraints_array_of(
    const index_sett &index_set, const array_of_exprt &exprt);
  void add_array_constraints_array_constant(
    const index_sett &index_set,
    const array_exprt &exprt);
  void add_array_constraints_comprehension(
    const index_sett &index_set,
    const array_comprehension_exprt &expr);

  void update_index_map(bool update_all);
  void update_index_map(std::size_t i);
  std::set<std::size_t> update_indices;
  void collect_arrays(const exprt &a);
  void collect_indices();
  void collect_indices(const exprt &a);
  std::unordered_set<irep_idt> array_comprehension_args;

  virtual bool is_unbounded_array(const typet &type) const=0;
    // (maybe this function should be partially moved here from boolbv)
};

#endif // CPROVER_SOLVERS_FLATTENING_ARRAYS_H
