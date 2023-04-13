// Author: Diffblue Ltd.

#include "goto_transform_history.h"

#include <util/invariant.h>

#include "goto_function.h"
#include "goto_functions.h"

#include <iostream>
#include <sstream> // IWYU pragma: keep

void goto_transform_historyt::add(goto_transform_kindt transform)
{
  INVARIANT(
    is_valid_transform_kind(transform),
    "History should be built using valid transformations.");
  _transforms.push_back(transform);
}

const goto_transform_historyt::transformst &
goto_transform_historyt::transforms() const
{
  return _transforms;
}

bool is_valid_transform_kind(const goto_transform_kindt transform)
{
  switch(transform)
  {
  case goto_transform_kindt::link_to_library:
  case goto_transform_kindt::string_instrumentation:
  case goto_transform_kindt::remove_function_pointers:
  case goto_transform_kindt::mm_io:
  case goto_transform_kindt::instrument_preconditions:
  case goto_transform_kindt::goto_partial_inline:
  case goto_transform_kindt::remove_returns:
  case goto_transform_kindt::remove_vector:
  case goto_transform_kindt::remove_complex:
  case goto_transform_kindt::rewrite_union:
  case goto_transform_kindt::transform_assertions_assumptions:
  case goto_transform_kindt::adjust_float_expressions:
  case goto_transform_kindt::string_abstraction:
  case goto_transform_kindt::remove_unused_functions:
  case goto_transform_kindt::remove_skip:
  case goto_transform_kindt::label_properties:
    return true;
    // Note: There is deliberately no `default:` case here in order to ensure
    // that there is a compilation error if the enum is updated without
    // updating the validation.
  }
  return false;
}

void add_history_transform(
  goto_transform_kindt transform_kind,
  goto_functiont &function)
{
  function.history.add(transform_kind);
}

void add_history_transform(
  goto_transform_kindt transform_kind,
  goto_functionst &functions)
{
  for(auto &function_pair : functions.function_map)
  {
    add_history_transform(transform_kind, function_pair.second);
  }
}

std::ostream &operator<<(std::ostream &os, const goto_transform_kindt &transform)
{
  switch(transform)
  {
    // Note: There is deliberately no `default:` case here in order to ensure
    // that there is a compilation error if the enum is updated without
    // updating the printing.
  case goto_transform_kindt::link_to_library:
    os << "link_to_library";
    break;
  case goto_transform_kindt::string_instrumentation:
    os << "string_instrumentation";
    break;
  case goto_transform_kindt::remove_function_pointers:
    os << "remove_function_pointers";
    break;
  case goto_transform_kindt::mm_io:
    os << "mm_io";
    break;
  case goto_transform_kindt::instrument_preconditions:
    os << "instrument_preconditions";
    break;
  case goto_transform_kindt::goto_partial_inline:
    os << "goto_partial_inline";
    break;
  case goto_transform_kindt::remove_returns:
    os << "remove_returns";
    break;
  case goto_transform_kindt::remove_vector:
    os << "remove_vector";
    break;
  case goto_transform_kindt::remove_complex:
    os << "remove_complex";
    break;
  case goto_transform_kindt::rewrite_union:
    os << "rewrite_union";
    break;
  case goto_transform_kindt::transform_assertions_assumptions:
    os << "transform_assertions_assumptions";
    break;
  case goto_transform_kindt::adjust_float_expressions:
    os << "adjust_float_expressions";
    break;
  case goto_transform_kindt::string_abstraction:
    os << "string_abstraction";
    break;
  case goto_transform_kindt::remove_unused_functions:
    os << "remove_unused_functions";
    break;
  case goto_transform_kindt::remove_skip:
    os << "remove_skip";
    break;
  case goto_transform_kindt::label_properties:
    os << "label_properties";
    break;
  }
  return os;
}

std::string to_string(const goto_transform_kindt &transform)
{
  std::stringstream ss;
  ss << transform;
  return ss.str();
}
