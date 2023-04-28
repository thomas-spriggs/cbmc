// Author: Fotis Koutoulakis for Diffblue Ltd.

#include "api_options.h"

// The following includes must be header-only, in order to avoid introducing
// linker dependencies.
#include <make_unique.h>
#include <optional.h>

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
  optionalt<std::string> external_sat_solver;
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
  if(implementation->external_sat_solver)
    return util_make_unique<std::string>(*implementation->external_sat_solver);
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

api_sat_optionst::buildert &api_sat_optionst::buildert::solver(api_sat_solvert solver)
{
  implementation->solver = solver;
  return *this;
}

api_sat_optionst::buildert &api_sat_optionst::buildert::external_sat_solver(
  std::string external_sat_solver)
{
  implementation->external_sat_solver = std::move(external_sat_solver);
  return *this;
}

api_sat_optionst::buildert &api_sat_optionst::buildert::sat_preprocessor(bool sat_preprocessor)
{
  implementation->sat_preprocessor = sat_preprocessor;
  return *this;
}

api_sat_optionst::buildert &api_sat_optionst::buildert::dimacs(bool dimacs)
{
  implementation->dimacs = dimacs;
  return *this;
}

struct api_legacy_smt_optionst::implementationt
{
		solvert solver_specialisation;
    /// Floating point arithmetic theory.
		bool use_FPA_theory;
};

api_legacy_smt_optionst::api_legacy_smt_optionst(
  std::unique_ptr<const implementationt> implementation)
: implementation{std::move(implementation)}
{
}

api_legacy_smt_optionst::solvert
api_legacy_smt_optionst::solver_specialisation()
{
    return implementation->solver_specialisation;
}

bool api_legacy_smt_optionst::use_FPA_theory()
{
    return implementation->use_FPA_theory;
}

api_legacy_smt_optionst::api_legacy_smt_optionst(
  api_legacy_smt_optionst &&api_options) noexcept = default;

api_legacy_smt_optionst::buildert::buildert() = default;
api_legacy_smt_optionst::buildert::buildert(
  api_legacy_smt_optionst::buildert &&builder) noexcept = default;
api_legacy_smt_optionst::buildert::~buildert() = default;

api_legacy_smt_optionst::~api_legacy_smt_optionst() = default;

api_legacy_smt_optionst api_legacy_smt_optionst::buildert::build()
{
  return api_legacy_smt_optionst(util_make_unique<implementationt>(*implementation));
}

api_legacy_smt_optionst::buildert &api_legacy_smt_optionst::buildert::solver_specialisation(
  api_legacy_smt_optionst::solvert solver)
{
  implementation->solver_specialisation = solver;
  return *this;
}

api_legacy_smt_optionst::buildert &api_legacy_smt_optionst::buildert::use_FPA_theory(bool use_enabled)
{
  implementation->use_FPA_theory = use_enabled;
  return *this;
}

struct api_incremental_smt_optionst::implementationt
{
  std::string solver_path;
};

api_incremental_smt_optionst::api_incremental_smt_optionst(
  std::unique_ptr<const implementationt> implementation)
: implementation{std::move(implementation)}
{
}

std::string api_incremental_smt_optionst::solver_path()
{
  return implementation->solver_path;
}

api_incremental_smt_optionst::~api_incremental_smt_optionst()= default;

api_incremental_smt_optionst::api_incremental_smt_optionst(
  api_incremental_smt_optionst &&api_options) noexcept = default;


api_incremental_smt_optionst::buildert::buildert() = default;
api_incremental_smt_optionst::buildert::buildert(
  api_incremental_smt_optionst::buildert &&builder) noexcept = default;
api_incremental_smt_optionst::buildert::~buildert() = default;
api_incremental_smt_optionst api_incremental_smt_optionst::buildert::build()
{
  return api_incremental_smt_optionst{util_make_unique<implementationt>(*implementation)};
}

api_incremental_smt_optionst::buildert &
api_incremental_smt_optionst::buildert::solver_path(std::string path)
{
  implementation->solver_path = std::move(path);
  return *this;
}

struct api_solver_optionst::implementationt
{
  optionalt<std::string> outfile;
  optionalt<std::string> write_solver_stats_to;
  bool beautify;
  optionalt<unsigned int> max_node_refinement;
  bool refine_arrays;
  bool refine_arthimetic;
};

api_solver_optionst::api_solver_optionst(
  std::unique_ptr<const implementationt> implementation)
  : implementation{std::move(implementation)}
{
}

api_solver_optionst::~api_solver_optionst() = default;
api_solver_optionst::api_solver_optionst(
  api_solver_optionst &&api_options) noexcept = default;

api_solver_optionst::buildert::buildert() = default;
api_solver_optionst::buildert::buildert(
  api_solver_optionst::buildert &&builder) noexcept = default;
api_solver_optionst::buildert::~buildert() = default;

api_solver_optionst api_solver_optionst::buildert::build()
{
  return api_solver_optionst{util_make_unique<implementationt>(*implementation)};
}

api_solver_optionst::buildert &api_solver_optionst::buildert::outfile(std::string outfile)
{
  implementation->outfile = std::move(outfile);
  return *this;
}

api_solver_optionst::buildert &api_solver_optionst::buildert::write_solver_stats_to(std::string filename)
{
  implementation->write_solver_stats_to = std::move(filename);
  return *this;
}

api_solver_optionst::buildert &api_solver_optionst::buildert::beautify(bool on)
{
  implementation->beautify = on;
  return *this;
}

api_solver_optionst::buildert &api_solver_optionst::buildert::max_node_refinement(unsigned int max_node_refinement)
{
  implementation->max_node_refinement = max_node_refinement;
  return *this;
}

api_solver_optionst::buildert &api_solver_optionst::buildert::refine_arrays(bool refine_arrays)
{
  implementation->refine_arrays = refine_arrays;
  return *this;
}

api_solver_optionst::buildert &api_solver_optionst::buildert::refine_arthimetic(bool refine_arthimetic)
{
  implementation->refine_arthimetic = refine_arthimetic;
  return *this;
}

