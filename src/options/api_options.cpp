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

api_sat_optionst::api_sat_optionst(const api_sat_optionst &api_options)
  : implementation{util_make_unique<implementationt>(*api_options.implementation)}
{
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
  const api_legacy_smt_optionst &api_options)
: implementation{util_make_unique<implementationt>(*api_options.implementation)}
{
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

api_incremental_smt_optionst::api_incremental_smt_optionst(
  const api_incremental_smt_optionst &api_options)
  : implementation{util_make_unique<implementationt>(*api_options.implementation)}
{
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
  // api_solver_optionst maintains the invariant that only one of the solver
  // types is specified.
  solver_typet solver_type;
  optionalt<api_sat_optionst> sat_options;
  optionalt<api_legacy_smt_optionst> legacy_smt_options;
  optionalt<api_incremental_smt_optionst> incremental_smt_options;

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

api_solver_optionst::buildert &
api_solver_optionst::buildert::api_sat_options(api_sat_optionst options)
{
  implementation->sat_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::api_sat_optionst;
  implementation->legacy_smt_options.reset();
  implementation->incremental_smt_options.reset();
  return *this;
}

api_solver_optionst::buildert &
api_solver_optionst::buildert::api_legacy_smt_options(api_legacy_smt_optionst options)
{
  implementation->legacy_smt_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::api_legacy_smt_optionst;
  implementation->sat_options.reset();
  implementation->incremental_smt_options.reset();
  return *this;
}

api_solver_optionst::buildert &
api_solver_optionst::buildert::api_incremental_smt_options(api_incremental_smt_optionst options)
{
  implementation->incremental_smt_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::api_incremental_smt_optionst;
  implementation->sat_options.reset();
  implementation->legacy_smt_options.reset();
  return *this;
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

std::unique_ptr<std::string> api_solver_optionst::outfile()
{
  if(implementation->outfile)
    return util_make_unique<std::string>(*implementation->outfile);
  else
    return nullptr;
}

std::unique_ptr<std::string> api_solver_optionst::write_solver_stats_to()
{
  if(implementation->write_solver_stats_to)
    return util_make_unique<std::string>(*implementation->write_solver_stats_to);
  else
    return nullptr;
}

bool api_solver_optionst::beautify()
{
  return implementation->beautify;
}

std::unique_ptr<unsigned int> api_solver_optionst::max_node_refinement()
{
  if(implementation->max_node_refinement)
    return util_make_unique<unsigned int>(*implementation->max_node_refinement);
  else
    return nullptr;
}

bool api_solver_optionst::refine_arrays()
{
  return implementation->refine_arrays;
}

bool api_solver_optionst::refine_arthimetic()
{
  return implementation->refine_arthimetic;
}
