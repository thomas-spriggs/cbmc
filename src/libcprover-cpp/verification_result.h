// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file
/// Interface for the various verification engines providing results in a structured form.

#ifndef CPROVER_GOTO_CHECKER_VERIFICATION_RESULT_H
#define CPROVER_GOTO_CHECKER_VERIFICATION_RESULT_H

#include "goto-checker/properties.h"

struct verification_resultt
{
  const propertiest properties;
  resultt verifier_result;

  explicit verification_resultt(
    propertiest &_properties,
    resultt _verifier_result)
    : properties(_properties),
      verifier_result(_verifier_result)
  {
  }
};

#endif // CPROVER_GOTO_CHECKER_VERIFICATION_RESULT_H
