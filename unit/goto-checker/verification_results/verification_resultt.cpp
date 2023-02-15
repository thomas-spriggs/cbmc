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
    GIVEN("A model with a passing property from C source code") {
        const std::string input = R"END(
            int main(int argc, char *argv[]) {
                int arr[] = {0, 1, 2, 3, 4};
                __CPROVER_assert(arr[3] == 3, "success expected: arr[3] is 3");
                return 0;
            })END";

        auto model = get_goto_model_from_c(input);

        REQUIRE(model.get_goto_functions().function_map.size() == 11);

        WHEN("We run that model past the verification engine") {
            const auto &results = run_verification_engine(model);

            THEN("The verification result should be success") {
                REQUIRE(results.verifier_result == resultt::PASS);
            }

            THEN("We should have access to the property and the property status should be PASS") {
                REQUIRE(results.properties.size() == 1);
                for (const auto &prop : results.properties) {
                    REQUIRE(prop.first == "main.assertion.1");
                    REQUIRE(prop.second.status == property_statust::PASS);
                }
            }
        }
    }

    GIVEN("A model with a failing property from C source code") {
        const std::string input = R"END(
            int main(int argc, char *argv[]) {
                int i;
                int arr[] = {0, 1, 2, 3, 4};
                __CPROVER_assert(arr[i] != 3, "failure expected: arr[3] is 3");
                return 0;
            })END";

        auto model = get_goto_model_from_c(input);

        REQUIRE(model.get_goto_functions().function_map.size() == 11);

        WHEN("We run that model past the verification engine") {
            const auto &results = run_verification_engine(model);

            THEN("The verification result should be failure") {
                REQUIRE(results.verifier_result == resultt::FAIL);
            }

            THEN("We should have access to the properties and the property status after the solver has run") {
                REQUIRE(results.properties.size() == 1);
                for (const auto &prop : results.properties) {
                    REQUIRE(prop.first == "main.assertion.1");
                    REQUIRE(prop.second.status == property_statust::FAIL);
                }
            }

            THEN("We should be able to get back a trace for the failed property") {
                const auto &trace = results.traces[dstringt("main.assertion.1")];
                REQUIRE(trace.steps.size() == 24);
            }
        }
    }
}
