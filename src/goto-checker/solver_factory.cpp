/*******************************************************************\

Module: Solver Factory

Author: Daniel Kroening, Peter Schrammel

\*******************************************************************/

/// \file
/// Solver Factory

#include "solver_factory.h"

#include <util/cmdline.h>
#include <util/exception_utils.h>
#include <util/exit_codes.h>
#include <util/make_unique.h>
#include <util/message.h>
#include <util/options.h>
#include <util/version.h>

#include "string2int.h"

#include <iostream>

#ifdef _MSC_VER
#include <util/unicode.h>
#endif

#include <solvers/stack_decision_procedure.h>

#include <solvers/flattening/bv_dimacs.h>
#include <solvers/prop/prop.h>
#include <solvers/prop/solver_resource_limits.h>
#include <solvers/refinement/bv_refinement.h>
#include <solvers/sat/dimacs_cnf.h>
#include <solvers/sat/external_sat.h>
#include <solvers/sat/satcheck.h>
#include <solvers/smt2_incremental/smt2_incremental_decision_procedure.h>
#include <solvers/smt2_incremental/smt_solver_process.h>
#include <solvers/strings/string_refinement.h>

#include <goto-symex/solver_hardness.h>

solver_factoryt::solver_factoryt(
  const optionst &_options,
  const namespacet &_ns,
  message_handlert &_message_handler,
  bool _output_xml_in_refinement)
  : options(_options),
    solver_options(*options.solver_options),
    ns(_ns),
    message_handler(_message_handler),
    output_xml_in_refinement(_output_xml_in_refinement)
{
}

solver_factoryt::solvert::solvert(std::unique_ptr<decision_proceduret> p)
  : decision_procedure_ptr(std::move(p))
{
}

solver_factoryt::solvert::solvert(
  std::unique_ptr<decision_proceduret> p1,
  std::unique_ptr<propt> p2)
  : prop_ptr(std::move(p2)), decision_procedure_ptr(std::move(p1))
{
}

solver_factoryt::solvert::solvert(
  std::unique_ptr<decision_proceduret> p1,
  std::unique_ptr<std::ofstream> p2)
  : ofstream_ptr(std::move(p2)), decision_procedure_ptr(std::move(p1))
{
}

decision_proceduret &solver_factoryt::solvert::decision_procedure() const
{
  PRECONDITION(decision_procedure_ptr != nullptr);
  return *decision_procedure_ptr;
}

stack_decision_proceduret &
solver_factoryt::solvert::stack_decision_procedure() const
{
  PRECONDITION(decision_procedure_ptr != nullptr);
  stack_decision_proceduret *solver =
    dynamic_cast<stack_decision_proceduret *>(&*decision_procedure_ptr);
  INVARIANT(solver != nullptr, "stack decision procedure required");
  return *solver;
}

void solver_factoryt::set_decision_procedure_time_limit(
  decision_proceduret &decision_procedure)
{
  const int timeout_seconds =
    options.get_signed_int_option("solver-time-limit");

  if(timeout_seconds > 0)
  {
    solver_resource_limitst *solver =
      dynamic_cast<solver_resource_limitst *>(&decision_procedure);
    if(solver == nullptr)
    {
      messaget log(message_handler);
      log.warning() << "cannot set solver time limit on "
                    << decision_procedure.decision_procedure_text()
                    << messaget::eom;
      return;
    }

    solver->set_time_limit_seconds(timeout_seconds);
  }
}

void solver_factoryt::solvert::set_decision_procedure(
  std::unique_ptr<decision_proceduret> p)
{
  decision_procedure_ptr = std::move(p);
}

void solver_factoryt::solvert::set_prop(std::unique_ptr<propt> p)
{
  prop_ptr = std::move(p);
}

void solver_factoryt::solvert::set_ofstream(std::unique_ptr<std::ofstream> p)
{
  ofstream_ptr = std::move(p);
}

std::unique_ptr<solver_factoryt::solvert> solver_factoryt::get_solver()
{
  std::unique_ptr<solver_factoryt::solvert> result = nullptr;
  solver_options.visit_solver_specific_options(
    [&](sat_optionst sat_options)
    {
      if(sat_options.dimacs())
      {
        result = get_dimacs();
        return;
      }
      if(const auto external_sat_solver = sat_options.external_sat_solver())
      {
        result = get_external_sat(*external_sat_solver);
        return;
      }
      if(options.get_bool_option("refine-strings"))
        result = get_string_refinement(sat_options);
      else if(solver_options.refine_arrays() || solver_options.refine_arthimetic())
      {
        result = get_bv_refinement(sat_options);
      }
    },
    [&](legacy_smt_optionst legacy_smt_options)
    {
      result = get_smt2(legacy_smt_options);
    },
    [&](incremental_smt_optionst incremental_smt_options)
    {
      result = get_incremental_smt2(incremental_smt_options);
    }
  );

  INVARIANT(result, "Implementation of get_solver should construct a solver.");
  return result;
}

/// Uses the options to pick an SMT 2.0 solver
/// \return An smt2_dect::solvert giving the solver to use.
static smt2_dect::solvert
get_smt2_solver_type(legacy_smt_optionst::solvert solver)
{
  switch(solver)
  {
    case legacy_smt_optionst::solvert::BITWUZLA:
      return smt2_dect::solvert::BITWUZLA;
    case legacy_smt_optionst::solvert::BOOLECTOR:
      return smt2_dect::solvert::BOOLECTOR;
    case legacy_smt_optionst::solvert::CPROVER_SMT2:
      return smt2_dect::solvert::CPROVER_SMT2;
    case legacy_smt_optionst::solvert::MATHSAT:
      return smt2_dect::solvert::MATHSAT;
    case legacy_smt_optionst::solvert::CVC3:
      return smt2_dect::solvert::CVC3;
    case legacy_smt_optionst::solvert::CVC4:
      return smt2_dect::solvert::CVC4;
    case legacy_smt_optionst::solvert::CVC5:
      return smt2_dect::solvert::CVC5;
    case legacy_smt_optionst::solvert::YICES:
      return smt2_dect::solvert::YICES;
    case legacy_smt_optionst::solvert::Z3:
      return smt2_dect::solvert::Z3;
    case legacy_smt_optionst::solvert::GENERIC:
      return smt2_dect::solvert::GENERIC;
    default:
      UNREACHABLE;
  }
}

/// Emit a warning for non-existent solver \p solver via \p message_handler.
static void emit_solver_warning(
  message_handlert &message_handler,
  const std::string &solver)
{
  messaget log(message_handler);
  log.warning() << "The specified solver, '" << solver
                << "', is not available. "
                << "The default solver will be used instead." << messaget::eom;
}

template <typename SatcheckT>
static std::unique_ptr<SatcheckT>
make_satcheck_prop(message_handlert &message_handler, const optionst &options)
{
  auto satcheck = util_make_unique<SatcheckT>(message_handler);
  if(options.is_set("write-solver-stats-to"))
  {
    if(
      auto hardness_collector = dynamic_cast<hardness_collectort *>(&*satcheck))
    {
      std::unique_ptr<solver_hardnesst> solver_hardness =
        util_make_unique<solver_hardnesst>();
      solver_hardness->set_outfile(options.get_option("write-solver-stats-to"));
      hardness_collector->solver_hardness = std::move(solver_hardness);
    }
    else
    {
      messaget log(message_handler);
      log.warning()
        << "Configured solver does not support --write-solver-stats-to. "
        << "Solver stats will not be written." << messaget::eom;
    }
  }
  return satcheck;
}

static std::unique_ptr<propt>
get_sat_solver(message_handlert &message_handler, const optionst &options,
               const sat_optionst &sat_options)
{
  const bool no_simplifier = options.solver_options->beautify() ||
                             sat_options.sat_preprocessor() ||
                             options.solver_options->refine_arthimetic() ||
                             options.solver_options->refine_arrays() ||
                             options.get_bool_option("refine-strings");

  if(options.is_set("sat-solver"))
  {
    const std::string &solver_option = options.get_option("sat-solver");
    if(solver_option == "zchaff")
    {
#if defined SATCHECK_ZCHAFF
      return make_satcheck_prop<satcheck_zchafft>(message_handler, options);
#else
      emit_solver_warning(message_handler, "zchaff");
#endif
    }
    else if(solver_option == "booleforce")
    {
#if defined SATCHECK_BOOLERFORCE
      return make_satcheck_prop<satcheck_booleforcet>(message_handler, options);
#else
      emit_solver_warning(message_handler, "booleforce");
#endif
    }
    else if(solver_option == "minisat1")
    {
#if defined SATCHECK_MINISAT1
      return make_satcheck_prop<satcheck_minisat1t>(message_handler, options);
#else
      emit_solver_warning(message_handler, "minisat1");
#endif
    }
    else if(solver_option == "minisat2")
    {
#if defined SATCHECK_MINISAT2
      if(no_simplifier)
      {
        // simplifier won't work with beautification
        return make_satcheck_prop<satcheck_minisat_no_simplifiert>(
          message_handler, options);
      }
      else // with simplifier
      {
        return make_satcheck_prop<satcheck_minisat_simplifiert>(
          message_handler, options);
      }
#else
      emit_solver_warning(message_handler, "minisat2");
#endif
    }
    else if(solver_option == "ipasir")
    {
#if defined SATCHECK_IPASIR
      return make_satcheck_prop<satcheck_ipasirt>(message_handler, options);
#else
      emit_solver_warning(message_handler, "ipasir");
#endif
    }
    else if(solver_option == "picosat")
    {
#if defined SATCHECK_PICOSAT
      return make_satcheck_prop<satcheck_picosatt>(message_handler, options);
#else
      emit_solver_warning(message_handler, "picosat");
#endif
    }
    else if(solver_option == "lingeling")
    {
#if defined SATCHECK_LINGELING
      return make_satcheck_prop<satcheck_lingelingt>(message_handler, options);
#else
      emit_solver_warning(message_handler, "lingeling");
#endif
    }
    else if(solver_option == "glucose")
    {
#if defined SATCHECK_GLUCOSE
      if(no_simplifier)
      {
        // simplifier won't work with beautification
        return make_satcheck_prop<satcheck_glucose_no_simplifiert>(
          message_handler, options);
      }
      else // with simplifier
      {
        return make_satcheck_prop<satcheck_glucose_simplifiert>(
          message_handler, options);
      }
#else
      emit_solver_warning(message_handler, "glucose");
#endif
    }
    else if(solver_option == "cadical")
    {
#if defined SATCHECK_CADICAL
      return make_satcheck_prop<satcheck_cadicalt>(message_handler, options);
#else
      emit_solver_warning(message_handler, "cadical");
#endif
    }
    else
    {
      messaget log(message_handler);
      log.error() << "unknown solver '" << solver_option << "'"
                  << messaget::eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
  }

  // default solver
  if(no_simplifier)
  {
    // simplifier won't work with beautification
    return make_satcheck_prop<satcheck_no_simplifiert>(
      message_handler, options);
  }
  else // with simplifier
  {
    return make_satcheck_prop<satcheckt>(message_handler, options);
  }
}

//std::unique_ptr<solver_factoryt::solvert> solver_factoryt::get_default()
//{
//  auto sat_solver = get_sat_solver(message_handler, options);
//
//  bool get_array_constraints =
//    options.get_bool_option("show-array-constraints");
//  auto bv_pointers = util_make_unique<bv_pointerst>(
//    ns, *sat_solver, message_handler, get_array_constraints);
//
//  if(options.get_option("arrays-uf") == "never")
//    bv_pointers->unbounded_array = bv_pointerst::unbounded_arrayt::U_NONE;
//  else if(options.get_option("arrays-uf") == "always")
//    bv_pointers->unbounded_array = bv_pointerst::unbounded_arrayt::U_ALL;
//
//  set_decision_procedure_time_limit(*bv_pointers);
//
//  return util_make_unique<solvert>(
//    std::move(bv_pointers), std::move(sat_solver));
//}

std::unique_ptr<solver_factoryt::solvert> solver_factoryt::get_dimacs()
{
  no_beautification();
  no_incremental_check();

  auto prop = util_make_unique<dimacs_cnft>(message_handler);

  const auto filename_option = solver_options.outfile();
  const std::string filename = filename_option ? *filename_option : "";

  auto bv_dimacs =
    util_make_unique<bv_dimacst>(ns, *prop, message_handler, filename);

  return util_make_unique<solvert>(std::move(bv_dimacs), std::move(prop));
}

std::unique_ptr<solver_factoryt::solvert>
  solver_factoryt::get_external_sat(const std::string &external_sat_solver)
{
  no_beautification();
  no_incremental_check();

  auto prop =
    util_make_unique<external_satt>(message_handler, external_sat_solver);

  auto bv_pointers = util_make_unique<bv_pointerst>(ns, *prop, message_handler);

  return util_make_unique<solvert>(std::move(bv_pointers), std::move(prop));
}

std::unique_ptr<solver_factoryt::solvert>
  solver_factoryt::get_bv_refinement(const sat_optionst &sat_options)
{
  std::unique_ptr<propt> prop = get_sat_solver(message_handler, options, sat_options);

  bv_refinementt::infot info;
  info.ns = &ns;
  info.prop = prop.get();
  info.output_xml = output_xml_in_refinement;

  // we allow setting some parameters
  if(options.get_bool_option("max-node-refinement"))
    info.max_node_refinement =
      options.get_unsigned_int_option("max-node-refinement");

  info.refine_arrays = solver_options.refine_arrays();
  info.refine_arithmetic = solver_options.refine_arthimetic();
  info.message_handler = &message_handler;

  auto decision_procedure = util_make_unique<bv_refinementt>(info);
  set_decision_procedure_time_limit(*decision_procedure);
  return util_make_unique<solvert>(
    std::move(decision_procedure), std::move(prop));
}

/// the string refinement adds to the bit vector refinement specifications for
/// functions from the Java string library
/// \return a solver for cbmc
std::unique_ptr<solver_factoryt::solvert>
solver_factoryt::get_string_refinement(const sat_optionst &sat_options)
{
  string_refinementt::infot info;
  info.ns = &ns;
  auto prop = get_sat_solver(message_handler, options, sat_options);
  info.prop = prop.get();
  info.refinement_bound = DEFAULT_MAX_NB_REFINEMENT;
  info.output_xml = output_xml_in_refinement;
  if(const auto max_node_refinement = solver_options.max_node_refinement())
    info.max_node_refinement = *max_node_refinement;
  info.refine_arrays = solver_options.refine_arrays();
  info.refine_arithmetic = solver_options.refine_arthimetic();
  info.message_handler = &message_handler;

  auto decision_procedure = util_make_unique<string_refinementt>(info);
  set_decision_procedure_time_limit(*decision_procedure);
  return util_make_unique<solvert>(
    std::move(decision_procedure), std::move(prop));
}

std::unique_ptr<std::ofstream> open_outfile_and_check(
  const std::string &filename,
  message_handlert &message_handler,
  const std::string &arg_name)
{
  if(filename.empty())
    return nullptr;

#ifdef _MSC_VER
  auto out = util_make_unique<std::ofstream>(widen(filename));
#else
  auto out = util_make_unique<std::ofstream>(filename);
#endif

  if(!*out)
  {
    throw invalid_command_line_argument_exceptiont(
      "failed to open file: " + filename, arg_name);
  }

  messaget log(message_handler);
  log.status() << "Outputting SMTLib formula to file: " << filename
               << messaget::eom;
  return out;
}

std::unique_ptr<solver_factoryt::solvert>
solver_factoryt::get_incremental_smt2(const incremental_smt_optionst &incremental_smt_options)
{
  no_beautification();

  const auto outfile_name = solver_options.outfile();
  const auto dump_smt_formula = incremental_smt_options.formula_dump_path();

  if(solver_options.outfile() && incremental_smt_options.formula_dump_path())
  {
    throw invalid_command_line_argument_exceptiont(
      "Argument --outfile is incompatible with --dump-smt-formula. ",
      "--outfile");
  }

  std::unique_ptr<smt_base_solver_processt> solver_process = nullptr;

  if(outfile_name)
  {
    bool on_std_out = *outfile_name == "-";
    std::unique_ptr<std::ostream> outfile =
      on_std_out
        ? nullptr
        : open_outfile_and_check(*outfile_name, message_handler, "--outfile");
    solver_process = util_make_unique<smt_incremental_dry_run_solvert>(
      message_handler, on_std_out ? std::cout : *outfile, std::move(outfile));
  }
  else
  {
    const auto out_filename = options.get_option("dump-smt-formula");

    // If no out_filename is provided `open_outfile_and_check` will return
    // `nullptr`, and the solver will work normally without any logging.
    solver_process = util_make_unique<smt_piped_solver_processt>(
      incremental_smt_options.solver_path(),
      message_handler,
      open_outfile_and_check(
        out_filename, message_handler, "--dump-smt-formula"));
  }

  return util_make_unique<solvert>(
    util_make_unique<smt2_incremental_decision_proceduret>(
      ns, std::move(solver_process), message_handler));
}

std::unique_ptr<solver_factoryt::solvert>
solver_factoryt::get_smt2(const legacy_smt_optionst &legacy_smt_options)
{
  no_beautification();

  const auto filename = solver_options.outfile();
  const auto solver =
    get_smt2_solver_type(legacy_smt_options.solver_specialisation());

  if(!filename)
  {
    if(legacy_smt_options.solver_specialisation() == legacy_smt_optionst::solvert::GENERIC)
    {
      throw invalid_command_line_argument_exceptiont(
        "required filename not provided",
        "--outfile",
        "provide a filename with --outfile");
    }

    auto smt2_dec = util_make_unique<smt2_dect>(
      ns,
      "cbmc",
      std::string("Generated by CBMC ") + CBMC_VERSION,
      "QF_AUFBV",
      solver,
      message_handler);

    if(options.get_bool_option("fpa"))
      smt2_dec->use_FPA_theory = true;

    set_decision_procedure_time_limit(*smt2_dec);
    return util_make_unique<solvert>(std::move(smt2_dec));
  }
  else if(*filename == "-")
  {
    auto smt2_conv = util_make_unique<smt2_convt>(
      ns,
      "cbmc",
      std::string("Generated by CBMC ") + CBMC_VERSION,
      "QF_AUFBV",
      solver,
      std::cout);

    if(options.get_bool_option("fpa"))
      smt2_conv->use_FPA_theory = true;

    set_decision_procedure_time_limit(*smt2_conv);
    return util_make_unique<solvert>(std::move(smt2_conv));
  }
  else
  {
    auto out = open_outfile_and_check(*filename, message_handler, "--outfile");

    auto smt2_conv = util_make_unique<smt2_convt>(
      ns,
      "cbmc",
      std::string("Generated by CBMC ") + CBMC_VERSION,
      "QF_AUFBV",
      solver,
      *out);

    if(options.get_bool_option("fpa"))
      smt2_conv->use_FPA_theory = true;

    set_decision_procedure_time_limit(*smt2_conv);
    return util_make_unique<solvert>(std::move(smt2_conv), std::move(out));
  }
}

void solver_factoryt::no_beautification()
{
  if(solver_options.beautify())
  {
    throw invalid_command_line_argument_exceptiont(
      "the chosen solver does not support beautification", "--beautify");
  }
}

void solver_factoryt::no_incremental_check()
{
  const bool all_properties = options.get_bool_option("all-properties");
  const bool cover = options.is_set("cover");
  const bool incremental_loop = options.is_set("incremental-loop");

  if(all_properties)
  {
    throw invalid_command_line_argument_exceptiont(
      "the chosen solver does not support incremental solving",
      "--all_properties");
  }
  else if(cover)
  {
    throw invalid_command_line_argument_exceptiont(
      "the chosen solver does not support incremental solving", "--cover");
  }
  else if(incremental_loop)
  {
    throw invalid_command_line_argument_exceptiont(
      "the chosen solver does not support incremental solving",
      "--incremental-loop");
  }
}

static void parse_sat_options(const cmdlinet &cmdline, optionst &options)
{
  if(cmdline.isset("external-sat-solver"))
  {
    options.set_option(
      "external-sat-solver", cmdline.get_value("external-sat-solver"));
  }

  options.set_option("sat-preprocessor", !cmdline.isset("no-sat-preprocessor"));

  if(cmdline.isset("dimacs"))
    options.set_option("dimacs", true);

  if(cmdline.isset("sat-solver"))
    options.set_option("sat-solver", cmdline.get_value("sat-solver"));
}

static sat_optionst
parse_sat_options(const cmdlinet &cmdline, message_handlert &message_handler)
{
  sat_optionst::buildert builder;
  if(cmdline.isset("external-sat-solver"))
  {
    builder.external_sat_solver(cmdline.get_value("external-sat-solver"));
  }

  builder.sat_preprocessor(!cmdline.isset("no-sat-preprocessor"));

  if(cmdline.isset("dimacs"))
    builder.dimacs(true);

  if(cmdline.isset("sat-solver"))
  {
    const std::string &solver_option = cmdline.get_value("sat-solver");
    if(solver_option == "zchaff")
      builder.solver(sat_solver_typest::zchaff);
    else if(solver_option == "booleforce")
      builder.solver(sat_solver_typest::booleforce);
    else if(solver_option == "minisat1")
      builder.solver(sat_solver_typest::minisat1);
    else if(solver_option == "minisat2")
      builder.solver(sat_solver_typest::minisat2);
    else if(solver_option == "ipasir")
      builder.solver(sat_solver_typest::ipasir);
    else if(solver_option == "picosat")
      builder.solver(sat_solver_typest::picosat);
    else if(solver_option == "lingeling")
      builder.solver(sat_solver_typest::lingeling);
    else if(solver_option == "glucose")
      builder.solver(sat_solver_typest::glucose);
    else if(solver_option == "cadical")
      builder.solver(sat_solver_typest::cadical);
    else
    {
      messaget log(message_handler);
      log.error() << "unknown solver '" << solver_option << "'"
                  << messaget::eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
  }
  return builder.build();
}

static void parse_smt2_options(const cmdlinet &cmdline, optionst &options)
{
  if(cmdline.isset("smt2"))
    options.set_option("smt2", true);

  if(cmdline.isset("fpa"))
    options.set_option("fpa", true);

  bool solver_set = false;

  if(cmdline.isset("bitwuzla"))
  {
    options.set_option("bitwuzla", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("boolector"))
  {
    options.set_option("boolector", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("cprover-smt2"))
  {
    options.set_option("cprover-smt2", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("mathsat"))
  {
    options.set_option("mathsat", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("cvc4"))
  {
    options.set_option("cvc4", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("cvc5"))
  {
    options.set_option("cvc5", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("incremental-smt2-solver"))
  {
    options.set_option(
      "incremental-smt2-solver", cmdline.get_value("incremental-smt2-solver")),
      solver_set = true;
  }

  if(cmdline.isset("yices"))
  {
    options.set_option("yices", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("z3"))
  {
    options.set_option("z3", true), solver_set = true;
    options.set_option("smt2", true);
  }

  if(cmdline.isset("smt2") && !solver_set)
  {
    if(cmdline.isset("outfile"))
    {
      // outfile and no solver should give standard compliant SMT-LIB
      options.set_option("generic", true);
    }
    else
    {
      // the default smt2 solver
      options.set_option("z3", true);
    }
  }

  if(cmdline.isset("dump-smt-formula"))
    options.set_option(
      "dump-smt-formula", cmdline.get_value("dump-smt-formula"));
}

static optionalt<legacy_smt_optionst>
  parse_legacy_smt2_options(const cmdlinet &cmdline)
{
  legacy_smt_optionst::buildert builder;

  if(cmdline.isset("fpa"))
    builder.use_FPA_theory(true);

  optionalt<legacy_smt_optionst::solvert> solver_specified;
  const auto parse_solver =
    [&](const char * const argument, const legacy_smt_optionst::solvert solver)
  {
    if(!cmdline.isset(argument))
      return;
    if(solver_specified)
      throw invalid_command_line_argument_exceptiont(
      "Multiple smt2 solvers were specified but only a single solver may be "
      "used at a time.", argument);
    solver_specified = solver;
  };
  parse_solver("bitwuzla", legacy_smt_optionst::solvert::BITWUZLA);
  parse_solver("boolector", legacy_smt_optionst::solvert::BOOLECTOR);
  parse_solver("cprover-smt2", legacy_smt_optionst::solvert::CPROVER_SMT2);
  parse_solver("mathsat", legacy_smt_optionst::solvert::MATHSAT);
  parse_solver("cvc4", legacy_smt_optionst::solvert::CVC4);
  parse_solver("cvc5", legacy_smt_optionst::solvert::CVC5);
  parse_solver("yices", legacy_smt_optionst::solvert::YICES);
  parse_solver("z3", legacy_smt_optionst::solvert::Z3);

  if(!cmdline.isset("smt2") && !solver_specified)
    return {};

  if(solver_specified)
  {
    builder.solver_specialisation(*solver_specified);
  }
  else if(cmdline.isset("outfile"))
  {
    // outfile and no solver should give standard compliant SMT-LIB
    builder.solver_specialisation(legacy_smt_optionst::solvert::GENERIC);
  }

  return {builder.build()};
}

static optionalt<incremental_smt_optionst>
  parse_incremental_smt2_options(const cmdlinet &cmdline)
{
  incremental_smt_optionst::buildert builder;

  if(!cmdline.isset("incremental-smt2-solver"))
    return {};
  builder.solver_path(cmdline.get_value("incremental-smt2-solver"));

  if(cmdline.isset("dump-smt-formula"))
    builder.formula_dump_path(cmdline.get_value("dump-smt-formula"));

  return {builder.build()};
}

void parse_solver_options(const cmdlinet &cmdline, optionst &options, message_handlert &message_handler)
{
  options.solver_options = parse_solver_options(cmdline, message_handler);

  parse_sat_options(cmdline, options);
  parse_smt2_options(cmdline, options);

  if(cmdline.isset("outfile"))
    options.set_option("outfile", cmdline.get_value("outfile"));

  if(cmdline.isset("write-solver-stats-to"))
  {
    options.set_option(
      "write-solver-stats-to", cmdline.get_value("write-solver-stats-to"));
  }

  if(cmdline.isset("beautify"))
    options.set_option("beautify", true);

  if(cmdline.isset("refine-arrays"))
  {
    options.set_option("refine", true);
    options.set_option("refine-arrays", true);
  }

  if(cmdline.isset("refine-arithmetic"))
  {
    options.set_option("refine", true);
    options.set_option("refine-arithmetic", true);
  }

  if(cmdline.isset("refine"))
  {
    options.set_option("refine", true);
    options.set_option("refine-arrays", true);
    options.set_option("refine-arithmetic", true);
  }

  if(cmdline.isset("max-node-refinement"))
  {
    options.set_option(
      "max-node-refinement", cmdline.get_value("max-node-refinement"));
  }
}

solver_optionst
parse_solver_options(const cmdlinet &cmdline, message_handlert &message_handler)
{
  solver_optionst::buildert builder;
  builder.sat_options(parse_sat_options(cmdline, message_handler));
  if(const auto legacy_smt2 = parse_legacy_smt2_options(cmdline))
    builder.legacy_smt_options(*legacy_smt2);
  if(const auto incremental_smt2 = parse_incremental_smt2_options(cmdline))
    builder.incremental_smt_options(*incremental_smt2);

  if(cmdline.isset("outfile"))
    builder.outfile(cmdline.get_value("outfile"));

  if(cmdline.isset("write-solver-stats-to"))
    builder.write_solver_stats_to(cmdline.get_value("write-solver-stats-to"));

  if(cmdline.isset("beautify"))
    builder.beautify(true);

  if(cmdline.isset("refine-arrays") || cmdline.isset("refine"))
    builder.refine_arrays(true);

  if(cmdline.isset("refine-arithmetic") || cmdline.isset("refine"))
    builder.refine_arthimetic(true);

  if(cmdline.isset("max-node-refinement"))
  {
    const std::string argument = cmdline.get_value("max-node-refinement");
    const auto parsed_value =
      string2optional<unsigned int>(cmdline.get_value("max-node-refinement"));
    if(!parsed_value)
    {
      messaget log(message_handler);
      log.error() << "Invalid value '" <<  argument
                  << "' specified for max-node-refinement."
                  << messaget::eom;
      exit(CPROVER_EXIT_USAGE_ERROR);
    }
    builder.max_node_refinement(*parsed_value);
  }

  return builder.build();
}
