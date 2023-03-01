// Copyright (c) 2023 Fotis Koutoulakis for Diffblue Ltd.

/// \file run_verification_engine.cpp
///       Provide a convenience function whose aim is to initialise the verification
///       engine and its dependencies, and run it against a passed in goto-model,
///       in the end returning a verification result type so that we can make assertions
///       against its structure.

#include "run_verification_engine.h"

#include <util/config.h>
#include <util/options.h>

#include <goto-programs/goto_model.h>
#include <goto-programs/link_to_library.h>
#include <goto-programs/process_goto_program.h>
#include <goto-programs/remove_skip.h>
#include <goto-programs/set_properties.h>

#include <ansi-c/cprover_library.h>
#include <assembler/remove_asm.h>
#include <goto-checker/all_properties_verifier_with_trace_storage.h>
#include <goto-checker/multi_path_symex_checker.h>
#include <goto-checker/properties.h>
#include <libcprover-cpp/verification_result.h>
#include <pointer-analysis/add_failed_symbols.h>

#include "message.h"

#include <sstream>

static optionst make_internal_default_options()
{
  optionst options = optionst();
  options.set_option("built-in-assertions", true);
  options.set_option("assertions", true);
  options.set_option("assumptions", true);
  options.set_option("arrays-uf", "auto");
  options.set_option("depth", UINT32_MAX);
  options.set_option("sat-preprocessor", true);
  options.set_option("trace", true);
  return options;
}

verification_resultt run_verification_engine(goto_modelt &model)
{
  // NOTE: This function has been modeled after the `api_sessiont::verify_model()` function
  // in the C++ API, in order to provide the minimum initialisation sequence for a verification
  // engine in a way that provides useful results (otherwise, VCCs may get pruned).

  // Initialise dependencies of verification engine

  // Initialise null message handler (acts like a drain, removing noisy
  // output from test output stream).
  std::ostringstream out;
  test_null_ui_message_handler null_ui_message_handler(out);
  messaget log{null_ui_message_handler};

  // Initialise options
  auto options = make_internal_default_options();

  // Perform dependent instrumentation passes before verification engine
  // runs.
  remove_asm(model);
  link_to_library(model, null_ui_message_handler, cprover_c_library_factory);
  process_goto_program(model, options, log);
  add_failed_symbols(model.symbol_table);
  label_properties(model);
  remove_skip(model);

  // Run verification engine and return the results
  all_properties_verifier_with_trace_storaget<multi_path_symex_checkert>
    verifier(options, null_ui_message_handler, model);

  auto res = verifier();
  auto props = verifier.get_properties();

  return verification_resultt{props, res, std::move(verifier.move_traces())};
}
