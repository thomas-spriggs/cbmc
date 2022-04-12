// Author: Diffblue Ltd.

#include <util/pointer_predicates.h>

#include <testing-utils/use_catch.h>

TEST_CASE("pointer_offset_exprt", "[core][util]")
{
  const exprt pointer = symbol_exprt{"foo", pointer_type(void_type())};
  const pointer_offset_exprt pointer_offset{pointer};
  SECTION("Is equivalent to free function.")
  {
    CHECK(::pointer_offset(pointer) == pointer_offset);
  }
  SECTION("Result type")
  {
    CHECK(pointer_offset.type() == signed_size_type());
  }
  SECTION("Pointer operand accessor")
  {
    CHECK(pointer_offset.pointer() == pointer);
  }
  SECTION("Downcasting")
  {
    const exprt &upcast = pointer_offset;
    CHECK(expr_try_dynamic_cast<pointer_offset_exprt>(pointer_offset));
    CHECK_FALSE(expr_try_dynamic_cast<object_size_exprt>(upcast));
    CHECK_FALSE(expr_try_dynamic_cast<pointer_object_exprt>(upcast));
  }
}

TEST_CASE("pointer_object_exprt", "[core][util]")
{
  const exprt pointer = symbol_exprt{"foo", pointer_type(void_type())};
  const pointer_object_exprt pointer_object{pointer};
  SECTION("Is equivalent to free function.")
  {
    CHECK(::pointer_object(pointer) == pointer_object);
  }
  SECTION("Result type")
  {
    CHECK(pointer_object.type() == size_type());
  }
  SECTION("Pointer operand accessor")
  {
    CHECK(pointer_object.pointer() == pointer);
  }
  SECTION("Downcasting")
  {
    const exprt &upcast = pointer_object;
    CHECK(expr_try_dynamic_cast<pointer_object_exprt>(pointer_object));
    CHECK_FALSE(expr_try_dynamic_cast<pointer_offset_exprt>(upcast));
    CHECK_FALSE(expr_try_dynamic_cast<object_size_exprt>(upcast));
  }
}

TEST_CASE("object_size_exprt", "[core][util]")
{
  const exprt pointer = symbol_exprt{"foo", pointer_type(void_type())};
  const object_size_exprt object_size{pointer};
  SECTION("Is equivalent to free function.")
  {
    CHECK(::object_size(pointer) == object_size);
  }
  SECTION("Result type")
  {
    CHECK(object_size.type() == size_type());
  }
  SECTION("Pointer operand accessor")
  {
    CHECK(object_size.pointer() == pointer);
  }
  SECTION("Downcasting")
  {
    const exprt &upcast = object_size;
    CHECK(expr_try_dynamic_cast<object_size_exprt>(object_size));
    CHECK_FALSE(expr_try_dynamic_cast<pointer_offset_exprt>(upcast));
    CHECK_FALSE(expr_try_dynamic_cast<pointer_object_exprt>(upcast));
  }
}
