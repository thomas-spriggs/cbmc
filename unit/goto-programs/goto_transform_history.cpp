// Author: Diffblue Ltd.

#include <goto-programs/goto_transform_history.h>

#include <testing-utils/use_catch.h>

TEST_CASE("Goto transform history data structure", "[core][goto-programs]")
{
  goto_transform_historyt history;
  REQUIRE(history.transforms().empty());
  history.add(goto_transform_kindt::mm_io);
  REQUIRE_THAT(
    history.transforms(),
    Catch::Matchers::Equals(
      goto_transform_historyt::transformst{goto_transform_kindt::mm_io}));
  history.add(goto_transform_kindt::adjust_float_expressions);
  REQUIRE_THAT(
    history.transforms(),
    Catch::Matchers::Equals(goto_transform_historyt::transformst{
      goto_transform_kindt::mm_io,
      goto_transform_kindt::adjust_float_expressions}));
}

TEST_CASE("Debug printing of individual history transform kinds",
          "[core][goto-programs]")
{
  CHECK(to_string(goto_transform_kindt::link_to_library) == "link_to_library");
  CHECK(to_string(goto_transform_kindt::string_instrumentation) == "string_instrumentation");
  CHECK(to_string(goto_transform_kindt::remove_function_pointers) == "remove_function_pointers");
  CHECK(to_string(goto_transform_kindt::mm_io) == "mm_io");
  CHECK(to_string(goto_transform_kindt::instrument_preconditions) == "instrument_preconditions");
  CHECK(to_string(goto_transform_kindt::goto_partial_inline) == "goto_partial_inline");
  CHECK(to_string(goto_transform_kindt::remove_returns) == "remove_returns");
  CHECK(to_string(goto_transform_kindt::remove_vector) == "remove_vector");
  CHECK(to_string(goto_transform_kindt::remove_complex) == "remove_complex");
  CHECK(to_string(goto_transform_kindt::rewrite_union) == "rewrite_union");
  CHECK(to_string(goto_transform_kindt::transform_assertions_assumptions) == "transform_assertions_assumptions");
  CHECK(to_string(goto_transform_kindt::adjust_float_expressions) == "adjust_float_expressions");
  CHECK(to_string(goto_transform_kindt::string_abstraction) == "string_abstraction");
  CHECK(to_string(goto_transform_kindt::remove_unused_functions) == "remove_unused_functions");
  CHECK(to_string(goto_transform_kindt::remove_skip) == "remove_skip");
  CHECK(to_string(goto_transform_kindt::label_properties) == "label_properties");
}
