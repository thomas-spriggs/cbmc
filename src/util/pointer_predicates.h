/*******************************************************************\

Module: Various predicates over pointers in programs

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Various predicates over pointers in programs

#ifndef CPROVER_UTIL_POINTER_PREDICATES_H
#define CPROVER_UTIL_POINTER_PREDICATES_H

#include "c_types.h"
#include "std_expr.h"

#define SYMEX_DYNAMIC_PREFIX "symex_dynamic::"

exprt same_object(const exprt &p1, const exprt &p2);
exprt deallocated(const exprt &pointer, const namespacet &);
exprt dead_object(const exprt &pointer, const namespacet &);
DEPRECATED(SINCE(2022, 4, 12, "use pointer_offset_exprt instead"))
exprt pointer_offset(const exprt &pointer);
DEPRECATED(SINCE(2022, 4, 12, "use pointer_object_exprt instead"))
exprt pointer_object(const exprt &pointer);
DEPRECATED(SINCE(2022, 4, 12, "use object_size_exprt instead"))
exprt object_size(const exprt &pointer);
exprt good_pointer(const exprt &pointer);
exprt good_pointer_def(const exprt &pointer, const namespacet &);
exprt null_object(const exprt &pointer);
exprt null_pointer(const exprt &pointer);
exprt integer_address(const exprt &pointer);
exprt object_lower_bound(
  const exprt &pointer,
  const exprt &offset);
exprt object_upper_bound(
  const exprt &pointer,
  const exprt &access_size);

/// Expression for extracting the offset component of a pointer.
class pointer_offset_exprt : public unary_exprt
{
public:
  explicit pointer_offset_exprt(exprt pointer)
    : unary_exprt(ID_pointer_offset, std::move(pointer), signed_size_type())
  {
  }

  const exprt &pointer() const
  {
    return op();
  }
};

template <>
inline bool can_cast_expr<pointer_offset_exprt>(const exprt &base)
{
  return base.id() == ID_pointer_offset;
}

/// Expression for extracting the object component of a pointer.
class pointer_object_exprt : public unary_exprt
{
public:
  explicit pointer_object_exprt(exprt pointer)
    : unary_exprt(ID_pointer_object, std::move(pointer), size_type())
  {
  }

  const exprt &pointer() const
  {
    return op();
  }
};

template <>
inline bool can_cast_expr<pointer_object_exprt>(const exprt &base)
{
  return base.id() == ID_pointer_object;
}

/// Expression for finding the size of the object a pointer points to.
class object_size_exprt : public unary_exprt
{
public:
  explicit object_size_exprt(exprt pointer)
    : unary_exprt(ID_object_size, std::move(pointer), size_type())
  {
  }

  const exprt &pointer() const
  {
    return op();
  }
};

template <>
inline bool can_cast_expr<object_size_exprt>(const exprt &base)
{
  return base.id() == ID_object_size;
}

class is_invalid_pointer_exprt : public unary_predicate_exprt
{
public:
  explicit is_invalid_pointer_exprt(exprt pointer)
    : unary_predicate_exprt{ID_is_invalid_pointer, std::move(pointer)}
  {
  }
};

template <>
inline bool can_cast_expr<is_invalid_pointer_exprt>(const exprt &base)
{
  return base.id() == ID_is_invalid_pointer;
}

inline void validate_expr(const is_invalid_pointer_exprt &value)
{
  validate_operands(value, 1, "is_invalid_pointer must have one operand");
}

#endif // CPROVER_UTIL_POINTER_PREDICATES_H
