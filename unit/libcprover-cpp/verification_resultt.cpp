// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file Integration tests for the the interface of the class for verification results
///       produced by various verification engines (classes downstream of
///       goto_verifiert).

#include <libcprover-cpp/verification_result.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../catch/catch.hpp"
#include "../testing-utils/get_goto_model_from_c.h"
#include "../testing-utils/message.h"
#include "../testing-utils/run_verification_engine.h"

SCENARIO(
  "When we analyse a model produced from some C code we get back verification "
  "results in a structured format",
  "[goto-checker][verification-result]")
{
  GIVEN("A model with a passing property from C source code")
  {
    const std::string input = R"END(
            int main(int argc, char *argv[]) {
                int arr[] = {0, 1, 2, 3, 4};
                __CPROVER_assert(arr[3] == 3, "success expected: arr[3] is 3");
                return 0;
            })END";

    auto model = get_goto_model_from_c(input);

    REQUIRE(model.get_goto_functions().function_map.size() == 11);

    WHEN("We run that model past the verification engine")
    {
      const verification_resultt &results = run_verification_engine(model);

      THEN("The verification result should be success")
      {
        REQUIRE(results.final_result() == verifier_resultt::PASS);
      }

      THEN(
        "We should have access to the property and the property status should "
        "be PASS")
      {
        auto properties = results.get_property_ids();
        REQUIRE(properties.size() == 1);
        REQUIRE(properties.at(0) == "main.assertion.1");

        THEN(
          "The property description should be the same as the assertion "
          "description in the model")
        {
          const std::string assertion_description{
            "success expected: arr[3] is 3"};
          const std::string results_description =
            results.get_property_description(properties.at(0));
          REQUIRE(assertion_description == results_description);
        }

        THEN("The property status should be PASS")
        {
          const auto property_status =
            results.get_property_status(properties.at(0));
          REQUIRE(property_status == prop_statust::PASS);
        }
      }
    }
  }

  GIVEN("A model with a failing property from C source code")
  {
    const std::string input = R"END(
            int main(int argc, char *argv[]) {
                int i;
                int arr[] = {0, 1, 2, 3, 4};
                __CPROVER_assert(arr[i] != 3, "failure expected: arr[3] is 3");
                return 0;
            })END";

    auto model = get_goto_model_from_c(input);

    REQUIRE(model.get_goto_functions().function_map.size() == 11);

    WHEN("We run that model past the verification engine")
    {
      const auto &results = run_verification_engine(model);

      THEN("The verification result should be failure")
      {
        REQUIRE(results.final_result() == verifier_resultt::FAIL);
      }

      // THEN(
      //   "We should have access to the properties and the property status after "
      //   "the solver has run")
      // {
      //   REQUIRE(results.properties.size() == 1);
      //   for(const auto &prop : results.properties)
      //   {
      //     REQUIRE(prop.first == "main.assertion.1");
      //     REQUIRE(prop.second.status == property_statust::FAIL);
      //   }
      // }
    }
  }
}
