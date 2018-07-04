/*******************************************************************\

Module: Struct abstract object

Author: Thomas Kiley, thomas.kiley@diffblue.com

\*******************************************************************/

#ifndef CPROVER_ANALYSES_VARIABLE_SENSITIVITY_FULL_STRUCT_ABSTRACT_OBJECT_H
#define CPROVER_ANALYSES_VARIABLE_SENSITIVITY_FULL_STRUCT_ABSTRACT_OBJECT_H

#include <stack>
#include <iosfwd>
#include <analyses/variable-sensitivity/abstract_object.h>
#include <analyses/variable-sensitivity/struct_abstract_object.h>
#include <util/sharing_map.h>

class abstract_environmentt;
class member_exprt;

class full_struct_abstract_objectt:public struct_abstract_objectt
{
public:
  typedef sharing_ptrt<full_struct_abstract_objectt> constant_struct_pointert;

  // Define an explicit copy constructor to ensure sharing of maps
  full_struct_abstract_objectt(const full_struct_abstract_objectt &ao);

  explicit full_struct_abstract_objectt(const typet &type);

  full_struct_abstract_objectt(const typet &type, bool top, bool bottom);

  full_struct_abstract_objectt(
    const exprt &expr,
    const abstract_environmentt &environment,
    const namespacet &ns);

  virtual void output(
    std::ostream &out,
    const class ai_baset &ai,
    const class namespacet &ns) const override;

  virtual abstract_object_pointert visit_sub_elements(
    const abstract_object_visitort &visitor) const override;

  void get_statistics(
    abstract_object_statisticst &statistics,
    abstract_object_visitedt &visited,
    const abstract_environmentt &env,
    const namespacet &ns) const override;

private:
  // no entry means component is top
  typedef sharing_mapt<irep_idt, abstract_object_pointert, irep_id_hash>
    shared_struct_mapt;
  shared_struct_mapt map;

  abstract_object_pointert merge_constant_structs(
    constant_struct_pointert other) const;

protected:
  CLONE

  // struct interface
  virtual abstract_object_pointert read_component(
    const abstract_environmentt &environment,
    const member_exprt &member_expr,
    const namespacet& ns) const override;

  virtual sharing_ptrt<struct_abstract_objectt> write_component(
    abstract_environmentt &environment,
    const namespacet &ns,
    const std::stack<exprt> &stack,
    const member_exprt &member_expr,
    const abstract_object_pointert value,
    bool merging_write) const override;

  bool verify() const;
  // Set the state of this to the merge result of op1 and op2 and
  // return if the result is different from op1
  virtual abstract_object_pointert merge(
    abstract_object_pointert other) const override;
};

#endif // CPROVER_ANALYSES_VARIABLE_SENSITIVITY_FULL_STRUCT_ABSTRACT_OBJECT_H
