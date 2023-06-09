// Author: Diffblue Ltd.

#include "struct_encoding.h"

#include <util/bitvector_expr.h>
#include <util/bitvector_types.h>
#include <util/make_unique.h>
#include <util/namespace.h>
#include <util/range.h>

#include <solvers/flattening/boolbv_width.h>

#include <algorithm>
#include <numeric>
#include <queue>

struct_encodingt::struct_encodingt(const namespacet &ns)
  : boolbv_width{util_make_unique<boolbv_widtht>(ns)}, ns{ns}
{
}

struct_encodingt::struct_encodingt(const struct_encodingt &other)
  : boolbv_width{util_make_unique<boolbv_widtht>(*other.boolbv_width)},
    ns{other.ns}
{
}

struct_encodingt::~struct_encodingt() = default;

typet struct_encodingt::encode(typet type) const
{
  std::queue<typet *> work_queue;
  work_queue.push(&type);
  while(!work_queue.empty())
  {
    typet &current = *work_queue.front();
    work_queue.pop();
    if(const auto struct_tag = type_try_dynamic_cast<struct_tag_typet>(current))
    {
      const auto width = (*boolbv_width)(*struct_tag);
      if(width == 0)
        UNIMPLEMENTED_FEATURE("support for zero bit width structs.");
      current = bv_typet{width};
    }
    if(const auto array = type_try_dynamic_cast<array_typet>(current))
    {
      work_queue.push(&array->element_type());
    }
  }
  return type;
}

/// \brief Extracts the component/field names and new values from a `with_exprt`
///        which expresses a new struct value where one or more members of a
///        struct have had their values substituted with new values.
/// \note  This is implemented using direct access to the operands and other
///        underlying irept interfaces, because the interface for `with_exprt`
///        only supports a single `where` / `new_value` pair and does not
///        support extracting the name from the `where` operand.
static std::unordered_map<irep_idt, exprt>
extricate_updates(const with_exprt &struct_expr)
{
  std::unordered_map<irep_idt, exprt> pairs;
  auto current_operand = struct_expr.operands().begin();
  // Skip the struct being updated in order to begin with the pairs.
  current_operand++;
  while(current_operand != struct_expr.operands().end())
  {
    INVARIANT(
      current_operand->id() == ID_member_name,
      "operand is expected to be the name of a member");
    auto name = current_operand->find(ID_component_name).id();
    ++current_operand;
    INVARIANT(
      current_operand != struct_expr.operands().end(),
      "every name is expected to be followed by a paired value");
    pairs[name] = *current_operand;
    ++current_operand;
  }
  POSTCONDITION(!pairs.empty());
  return pairs;
}

static exprt encode(const with_exprt &with, const namespacet &ns)
{
  const auto tag_type = type_checked_cast<struct_tag_typet>(with.type());
  const auto struct_type =
    type_checked_cast<struct_typet>(ns.follow(with.type()));
  const auto updates = extricate_updates(with);
  const auto components = make_range(struct_type.components())
    .map([&](const struct_union_typet::componentt &component) -> exprt {
      const auto &update = updates.find(component.get_name());
      if(update != updates.end())
        return update->second;
      else
        return member_exprt{with.old(), component.get_name(), component.type()};
    })
    .collect<exprt::operandst>();
  return struct_exprt{components, tag_type};
}

static exprt encode(const struct_exprt &struct_expr)
{
  INVARIANT(
    !struct_expr.operands().empty(),
    "empty structs may not be encoded into SMT terms.");
  if(struct_expr.operands().size() == 1)
    return struct_expr.operands().front();
  return concatenation_exprt{struct_expr.operands(), struct_expr.type()};
}

static std::size_t count_trailing_bit_width(
  const struct_typet &type,
  const irep_idt name,
  const boolbv_widtht &boolbv_width)
{
  auto member_component_rit = std::find_if(
    type.components().rbegin(),
    type.components().rend(),
    [&](const struct_union_typet::componentt &component) {
      return component.get_name() == name;
    });
  INVARIANT(
    member_component_rit != type.components().rend(),
    "Definition of struct type should include named component.");
  const auto trailing_widths =
    make_range(type.components().rbegin(), member_component_rit)
      .map([&](const struct_union_typet::componentt &component) -> std::size_t {
        return boolbv_width(component.type());
      });
  return std::accumulate(
    trailing_widths.begin(), trailing_widths.end(), std::size_t{0});
}

/// The member expression selects the value of a field from a struct. The
/// struct is encoded as a single bitvector where the first field is stored
/// in the highest bits. The second field is stored in the next highest set of
/// bits and so on. As offsets are indexed from the lowest bit, any field can be
/// selected by extracting the range of bits starting from an offset based on
/// the combined width of the fields which follow the field being selected.
exprt struct_encodingt::encode_member(const member_exprt &member_expr) const
{
  const auto &struct_type = type_checked_cast<struct_typet>(
    ns.get().follow(member_expr.compound().type()));
  const std::size_t offset_bits = count_trailing_bit_width(
    struct_type, member_expr.get_component_name(), *boolbv_width);
  const auto member_bits_width = (*boolbv_width)(member_expr.type());
  return extractbits_exprt{
    member_expr.compound(),
    offset_bits + member_bits_width - 1,
    offset_bits,
    member_expr.type()};
}

exprt struct_encodingt::encode(exprt expr) const
{
  std::queue<exprt *> work_queue;
  work_queue.push(&expr);
  while(!work_queue.empty())
  {
    exprt &current = *work_queue.front();
    work_queue.pop();
    // Note that "with" expressions are handled before type encoding in order to
    // facilitate checking that they are applied to structs rather than arrays.
    if(const auto with_expr = expr_try_dynamic_cast<with_exprt>(current))
      if(can_cast_type<struct_tag_typet>(current.type()))
        current = ::encode(*with_expr, ns);
    current.type() = encode(current.type());
    if(const auto struct_expr = expr_try_dynamic_cast<struct_exprt>(current))
      current = ::encode(*struct_expr);
    if(const auto member_expr = expr_try_dynamic_cast<member_exprt>(current))
      current = encode_member(*member_expr);
    for(auto &operand : current.operands())
      work_queue.push(&operand);
  }
  return expr;
}
