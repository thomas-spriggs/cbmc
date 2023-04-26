// Author: Fotis Koutoulakis for Diffblue Ltd.

#include "api_options.h"

#include <util/make_unique.h>

struct api_optionst::implementationt
{
  // Options for the verification engine
  bool simplify_enabled;

  // Option for dropping unused function
  bool drop_unused_functions_enabled;

  // Option for validating the goto model
  bool validate_goto_model_enabled;
};

api_optionst::api_optionst(
  std::unique_ptr<const implementationt> implementation)
: implementation{std::move(implementation)}
{
}

bool api_optionst::simplify() const
{
  return implementation->simplify_enabled;
}

bool api_optionst::drop_unused_functions() const
{
  return implementation->drop_unused_functions_enabled;
}

bool api_optionst::validate_goto_model() const
{
  return implementation->validate_goto_model_enabled;
}

api_optionst::api_optionst(api_optionst &&api_options) noexcept = default;
api_optionst::~api_optionst() = default;

api_optionst::buildert &api_optionst::buildert::simplify(bool on)
{
  implementation->simplify_enabled = on;
  return *this;
}

api_optionst::buildert &api_optionst::buildert::drop_unused_functions(bool on)
{
  implementation->drop_unused_functions_enabled = on;
  return *this;
}

api_optionst::buildert &api_optionst::buildert::validate_goto_model(bool on)
{
  implementation->validate_goto_model_enabled = on;
  return *this;
}

api_optionst api_optionst::buildert::build()
{
  auto impl = util_make_unique<implementationt>(*implementation);
  api_optionst api_options{std::move(impl)};
  return api_options;
}

api_optionst::buildert::buildert() = default;
api_optionst::buildert::buildert(api_optionst::buildert &&builder) noexcept = default;
api_optionst::buildert::~buildert() = default;

struct api_sat_optionst::implementationt
{
  api_sat_solvert solver = api_sat_solvert::minisat2;
  bool using_external_sat_solver = false;
  std::string external_sat_solver;
  bool sat_preprocessor = true;
  bool dimacs = false;
};

api_sat_optionst::api_sat_optionst(
  std::unique_ptr<const implementationt> implementation)
  : implementation{std::move(implementation)}
{
}

api_sat_optionst::api_sat_optionst(api_sat_optionst &&api_options) noexcept
  : implementation{std::move(api_options.implementation)}
{
}

api_sat_solvert api_sat_optionst::solver()
{
  return implementation->solver;
}

std::unique_ptr<std::string> api_sat_optionst::external_sat_solver()
{
  if(implementation->using_external_sat_solver)
    return util_make_unique<std::string>(implementation->external_sat_solver);
  else
    return nullptr;
}

bool api_sat_optionst::sat_preprocessor()
{
  return implementation->sat_preprocessor;
}

bool api_sat_optionst::dimacs()
{
  return implementation->dimacs;
}

api_sat_optionst::~api_sat_optionst() = default;

api_sat_optionst::buildert::buildert() = default;

api_sat_optionst::buildert::buildert(
  api_sat_optionst::buildert &&builder) noexcept = default;
api_sat_optionst::buildert::~buildert() = default;

api_sat_optionst api_sat_optionst::buildert::build()
{
  auto impl = util_make_unique<implementationt>(*implementation);
  api_sat_optionst api_sat_options{std::move(impl)};
  return api_sat_options;
}

void api_sat_optionst::buildert::solver(api_sat_solvert solver)
{
  this->implementation->solver = solver;
}

void api_sat_optionst::buildert::external_sat_solver(
  std::string external_sat_solver)
{
  implementation->using_external_sat_solver = true;
  implementation->external_sat_solver = std::move(external_sat_solver);
}

void api_sat_optionst::buildert::sat_preprocessor(bool sat_preprocessor)
{
  implementation->sat_preprocessor = sat_preprocessor;
}

void api_sat_optionst::buildert::dimacs(bool dimacs)
{
  implementation->dimacs = dimacs;
}
