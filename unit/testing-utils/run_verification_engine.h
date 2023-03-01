// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file run_verification_engine.cpp
///       Provide a convenience function whose aim is to initialise the verification
///       engine and its dependencies, and run it against a passed in goto-model,
///       in the end returning a verification result type so that we can make assertions
///       against its structure.

#ifndef CPROVER_TESTING_UTILS_RUN_VERIFICATION_ENGINE_H
#define CPROVER_TESTING_UTILS_RUN_VERIFICATION_ENGINE_H

#include <goto-programs/goto_model.h>

#include <libcprover-cpp/verification_result.h>

/// Initiate the verification engine and its dependencies and perform a simple
/// verification run against the provided goto-model, returning the results
/// in a structured way.
verification_resultt run_verification_engine(goto_modelt &model);

#endif // CPROVER_TESTING_UTILS_RUN_VERIFICATION_ENGINE_H
