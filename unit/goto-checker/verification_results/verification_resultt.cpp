// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file Integration tests for the the interface of the class for verification results
///       produced by various verification engines (classes downstream of 
///       goto_verifiert).

#include <iostream>
#include <string>
#include <sstream>

#include <goto-checker/multi_path_symex_checker.h>
#include <goto-checker/all_properties_verifier_with_trace_storage.h>
#include <goto-checker/properties.h>

#include <testing-utils/use_catch.h>
#include <testing-utils/get_goto_model_from_c.h>
#include <testing-utils/run_verification_engine.h>
#include <testing-utils/message.h>

SCENARIO("When we analyse a model produced from some C code we get back verification results in a structured format", "[goto-checker][verification-result]")
{
    GIVEN("A model from some C source code") {
        const std::string input = R"END(
            int main(int argc, char *argv[]) {
                int arr[] = {0, 1, 2, 3, 4};
                __CPROVER_assert(arr[3] != 3, "failure expected: arr[3] is 3");
                return 0;
            })END";

        auto model = get_goto_model_from_c(input);

        REQUIRE(model.get_goto_functions().function_map.size() == 11);

        WHEN("We run that model past the verification engines") {
            auto s = run_verification_engine(model);

            THEN("The verification result should be success") {
                REQUIRE(s.verifier_result == resultt::FAIL);
            }

            THEN("We should have access to the properties and certain information about them") {
                REQUIRE(s.properties.size() == 1);
                for ( const auto &prop : s.properties ) {
                    REQUIRE(prop.second.status == property_statust::FAIL);
                }
            }
        }
    }
}
