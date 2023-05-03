// Author: Thomas Spriggs for Diffblue Ltd.

#include "solver_options.h"

// The following includes must be header-only, in order to avoid introducing
// linker dependencies.
#include <make_unique.h>
#include <optional.h>

struct sat_optionst::implementationt
{
  sat_solver_typest solver = sat_solver_typest::minisat2;
  optionalt<std::string> external_sat_solver;
  bool sat_preprocessor = true;
  bool dimacs = false;
};

sat_optionst::sat_optionst(
  std::unique_ptr<const implementationt> implementation)
  : implementation{std::move(implementation)}
{
}

sat_optionst::sat_optionst(sat_optionst &&options) noexcept = default;
sat_optionst &sat_optionst::operator=(sat_optionst &&options) noexcept = default;

sat_solver_typest sat_optionst::solver() const
{
  return implementation->solver;
}

std::unique_ptr<std::string> sat_optionst::external_sat_solver() const
{
  if(implementation->external_sat_solver)
    return util_make_unique<std::string>(*implementation->external_sat_solver);
  else
    return nullptr;
}

bool sat_optionst::sat_preprocessor() const
{
  return implementation->sat_preprocessor;
}

bool sat_optionst::dimacs() const
{
  return implementation->dimacs;
}

sat_optionst::sat_optionst(const sat_optionst &options)
  : implementation{util_make_unique<implementationt>(*options.implementation)}
{
}

sat_optionst &sat_optionst::operator=(const sat_optionst &options)
{
  implementation = util_make_unique<implementationt>(*options.implementation);
  return *this;
}

sat_optionst::~sat_optionst() = default;

sat_optionst::buildert::buildert() = default;

sat_optionst::buildert::buildert(
  sat_optionst::buildert &&builder) noexcept = default;
sat_optionst::buildert &
sat_optionst::buildert::operator=(sat_optionst::buildert &&builder) noexcept = default;
sat_optionst::buildert::~buildert() = default;

sat_optionst sat_optionst::buildert::build()
{
  auto impl = util_make_unique<implementationt>(*implementation);
  sat_optionst sat_options{std::move(impl)};
  return sat_options;
}

sat_optionst::buildert &sat_optionst::buildert::solver(sat_solver_typest solver)
{
  implementation->solver = solver;
  return *this;
}

sat_optionst::buildert &sat_optionst::buildert::external_sat_solver(
  std::string external_sat_solver)
{
  implementation->external_sat_solver = std::move(external_sat_solver);
  return *this;
}

sat_optionst::buildert &sat_optionst::buildert::sat_preprocessor(bool sat_preprocessor)
{
  implementation->sat_preprocessor = sat_preprocessor;
  return *this;
}

sat_optionst::buildert &sat_optionst::buildert::dimacs(bool dimacs)
{
  implementation->dimacs = dimacs;
  return *this;
}

struct legacy_smt_optionst::implementationt
{
		solvert solver_specialisation;
    /// Floating point arithmetic theory.
		bool use_FPA_theory;
};

legacy_smt_optionst::legacy_smt_optionst(
  std::unique_ptr<const implementationt> implementation)
: implementation{std::move(implementation)}
{
}

legacy_smt_optionst::solvert
legacy_smt_optionst::solver_specialisation() const
{
    return implementation->solver_specialisation;
}

bool legacy_smt_optionst::use_FPA_theory() const
{
    return implementation->use_FPA_theory;
}

legacy_smt_optionst::legacy_smt_optionst(
  const legacy_smt_optionst &options)
: implementation{util_make_unique<implementationt>(*options.implementation)}
{
}

legacy_smt_optionst &
legacy_smt_optionst::operator=(const legacy_smt_optionst &options)
{
  implementation = util_make_unique<implementationt>(*options.implementation);
  return *this;
}

legacy_smt_optionst::legacy_smt_optionst(
  legacy_smt_optionst &&options) noexcept = default;
legacy_smt_optionst &
legacy_smt_optionst::operator=(legacy_smt_optionst &&options) noexcept = default;

legacy_smt_optionst::buildert::buildert() = default;
legacy_smt_optionst::buildert::buildert(
  legacy_smt_optionst::buildert &&builder) noexcept = default;
legacy_smt_optionst::buildert &legacy_smt_optionst::buildert::operator=(
  legacy_smt_optionst::buildert &&builder) noexcept = default;
legacy_smt_optionst::buildert::~buildert() = default;

legacy_smt_optionst::~legacy_smt_optionst() = default;

legacy_smt_optionst legacy_smt_optionst::buildert::build()
{
  return legacy_smt_optionst(util_make_unique<implementationt>(*implementation));
}

legacy_smt_optionst::buildert &legacy_smt_optionst::buildert::solver_specialisation(
  legacy_smt_optionst::solvert solver)
{
  implementation->solver_specialisation = solver;
  return *this;
}

legacy_smt_optionst::buildert &legacy_smt_optionst::buildert::use_FPA_theory(bool use_enabled)
{
  implementation->use_FPA_theory = use_enabled;
  return *this;
}

struct incremental_smt_optionst::implementationt
{
  std::string solver_path;
  optionalt<std::string> formula_dump_path;
};

incremental_smt_optionst::incremental_smt_optionst(
  std::unique_ptr<const implementationt> implementation)
: implementation{std::move(implementation)}
{
}

std::string incremental_smt_optionst::solver_path() const
{
  return implementation->solver_path;
}

std::unique_ptr<std::string> incremental_smt_optionst::formula_dump_path() const
{
  if(implementation->formula_dump_path)
    return util_make_unique<std::string>(*implementation->formula_dump_path);
  else
    return nullptr;
}

incremental_smt_optionst::incremental_smt_optionst(
  const incremental_smt_optionst &options)
  : implementation{util_make_unique<implementationt>(*options.implementation)}
{
}
incremental_smt_optionst &
incremental_smt_optionst::operator=(const incremental_smt_optionst &)
{
  implementation = util_make_unique<implementationt>(*implementation);
  return *this;
}

incremental_smt_optionst::~incremental_smt_optionst()= default;

incremental_smt_optionst::incremental_smt_optionst(
  incremental_smt_optionst &&options) noexcept = default;
incremental_smt_optionst &
incremental_smt_optionst::operator=(incremental_smt_optionst &&) noexcept = default;



incremental_smt_optionst::buildert::buildert() = default;
incremental_smt_optionst::buildert::buildert(
  incremental_smt_optionst::buildert &&builder) noexcept = default;
incremental_smt_optionst::buildert &
incremental_smt_optionst::buildert::operator=(
  incremental_smt_optionst::buildert &&builder) noexcept = default;
incremental_smt_optionst::buildert::~buildert() = default;
incremental_smt_optionst incremental_smt_optionst::buildert::build()
{
  return incremental_smt_optionst{util_make_unique<implementationt>(*implementation)};
}

incremental_smt_optionst::buildert &
incremental_smt_optionst::buildert::solver_path(std::string path)
{
  implementation->solver_path = std::move(path);
  return *this;
}

incremental_smt_optionst::buildert &
incremental_smt_optionst::buildert::formula_dump_path(std::string path)
{
  implementation->formula_dump_path = std::move(path);
  return *this;
}

struct solver_optionst::implementationt
{
  // solver_optionst maintains the invariant that only one of the solver
  // types is specified.
  solver_typet solver_type = solver_typet::sat;
  optionalt<sat_optionst> sat_options = sat_optionst::buildert{}.build();
  optionalt<legacy_smt_optionst> legacy_smt_options;
  optionalt<incremental_smt_optionst> incremental_smt_options;

  optionalt<std::string> outfile;
  optionalt<std::string> write_solver_stats_to;
  bool beautify = false;
  optionalt<unsigned int> max_node_refinement;
  bool refine_arrays = false;
  bool refine_arthimetic = false;
};

solver_optionst::solver_optionst(
  std::unique_ptr<const implementationt> implementation)
  : implementation{std::move(implementation)}
{
}

solver_optionst::~solver_optionst() = default;
solver_optionst::solver_optionst(
  solver_optionst &&options) noexcept = default;
solver_optionst::solver_optionst(const solver_optionst &options)
: implementation{util_make_unique<implementationt>(*options.implementation)}
{
}
solver_optionst &
  solver_optionst::operator=(solver_optionst &&) noexcept = default;
solver_optionst &solver_optionst::operator=(const solver_optionst &options)
{
  implementation = util_make_unique<implementationt>(*options.implementation);
  return *this;
}

solver_optionst::buildert::buildert() = default;
solver_optionst::buildert::buildert(
  solver_optionst::buildert &&builder) noexcept = default;
solver_optionst::buildert &solver_optionst::buildert::operator=(
  solver_optionst::buildert &&builder) noexcept = default;
solver_optionst::buildert::~buildert() = default;

solver_optionst solver_optionst::buildert::build()
{
  return solver_optionst{util_make_unique<implementationt>(*implementation)};
}

solver_optionst::buildert &
solver_optionst::buildert::sat_options(sat_optionst options)
{
  implementation->sat_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::sat;
  implementation->legacy_smt_options.reset();
  implementation->incremental_smt_options.reset();
  return *this;
}

solver_optionst::buildert &
solver_optionst::buildert::legacy_smt_options(legacy_smt_optionst options)
{
  implementation->legacy_smt_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::legacy_smt;
  implementation->sat_options.reset();
  implementation->incremental_smt_options.reset();
  return *this;
}

solver_optionst::buildert &
solver_optionst::buildert::incremental_smt_options(incremental_smt_optionst options)
{
  implementation->incremental_smt_options.emplace(std::move(options));
  implementation->solver_type = solver_typet::incremental_smt;
  implementation->sat_options.reset();
  implementation->legacy_smt_options.reset();
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::outfile(std::string outfile)
{
  implementation->outfile = std::move(outfile);
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::write_solver_stats_to(std::string filename)
{
  implementation->write_solver_stats_to = std::move(filename);
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::beautify(bool on)
{
  implementation->beautify = on;
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::max_node_refinement(unsigned int max_node_refinement)
{
  implementation->max_node_refinement = max_node_refinement;
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::refine_arrays(bool refine_arrays)
{
  implementation->refine_arrays = refine_arrays;
  return *this;
}

solver_optionst::buildert &solver_optionst::buildert::refine_arthimetic(bool refine_arthimetic)
{
  implementation->refine_arthimetic = refine_arthimetic;
  return *this;
}

std::unique_ptr<std::string> solver_optionst::outfile() const
{
  if(implementation->outfile)
    return util_make_unique<std::string>(*implementation->outfile);
  else
    return nullptr;
}

std::unique_ptr<std::string> solver_optionst::write_solver_stats_to() const
{
  if(implementation->write_solver_stats_to)
    return util_make_unique<std::string>(*implementation->write_solver_stats_to);
  else
    return nullptr;
}

bool solver_optionst::beautify() const
{
  return implementation->beautify;
}

std::unique_ptr<unsigned int> solver_optionst::max_node_refinement() const
{
  if(implementation->max_node_refinement)
    return util_make_unique<unsigned int>(*implementation->max_node_refinement);
  else
    return nullptr;
}

bool solver_optionst::refine_arrays() const
{
  return implementation->refine_arrays;
}

bool solver_optionst::refine_arthimetic() const
{
  return implementation->refine_arthimetic;
}

void solver_optionst::visit_solver_specific_options(
    const std::function<void(sat_optionst)> &sat_visitor,
    const std::function<void(legacy_smt_optionst)> &legacy_smt_visitor,
    const std::function<void(incremental_smt_optionst)> &legacy_smt_vistor) const
{
  switch(implementation->solver_type)
  {
  case solver_typet::sat:
    sat_visitor(*implementation->sat_options);
    return;
  case solver_typet::legacy_smt:
    legacy_smt_visitor(*implementation->legacy_smt_options);
    return;
  case solver_typet::incremental_smt:
    legacy_smt_vistor(*implementation->incremental_smt_options);
    return;
  }
}
