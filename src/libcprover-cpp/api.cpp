// Author: Fotis Koutoulakis for Diffblue Ltd.

#include "api.h"

#include <util/cmdline.h>
#include <util/config.h>
#include <util/invariant.h>
#include <util/message.h>
#include <util/options.h>
#include <util/ui_message.h>
#include <util/version.h>

#include <goto-programs/goto_model.h>
#include <goto-programs/initialize_goto_model.h>
#include <goto-programs/link_to_library.h>
#include <goto-programs/process_goto_program.h>
#include <goto-programs/remove_skip.h>
#include <goto-programs/remove_unused_functions.h>
#include <goto-programs/set_properties.h>

#include <ansi-c/ansi_c_language.h>
#include <ansi-c/cprover_library.h>
#include <ansi-c/gcc_version.h>
#include <assembler/remove_asm.h>
#include <goto-checker/all_properties_verifier_with_trace_storage.h>
#include <goto-checker/multi_path_symex_checker.h>
#include <langapi/mode.h>
#include <pointer-analysis/add_failed_symbols.h>

#include "goto_check_c.h"

#include <memory>
#include <string>
#include <vector>

extern configt config;
static std::unique_ptr<optionst> to_engine_options(const api_optionst &api_options);

std::unique_ptr<std::string> api_sessiont::get_api_version() const
{
  return util_make_unique<std::string>(std::string{CBMC_VERSION});
}

struct api_session_implementationt
{
  std::unique_ptr<goto_modelt> model;
  std::unique_ptr<message_handlert> message_handler;
  std::unique_ptr<optionst> options;
};

api_sessiont::api_sessiont() : api_sessiont{api_optionst::buildert{}.build()}
{
}

api_sessiont::api_sessiont(const api_optionst &options)
  : implementation{util_make_unique<api_session_implementationt>()}
{
  implementation->message_handler =
    util_make_unique<null_message_handlert>(null_message_handlert{});
  implementation->options = to_engine_options(options);
  // Needed to initialise the language options correctly
  cmdlinet cmdline;
  // config is global in config.cpp
  config.set(cmdline);
  // Initialise C language mode
  register_language(new_ansi_c_language);
  // configure gcc, if required -- get_goto_program will have set the config
  if(config.ansi_c.preprocessor == configt::ansi_ct::preprocessort::GCC)
  {
    gcc_versiont gcc_version;
    gcc_version.get("gcc");
    configure_gcc(gcc_version);
  }
}

api_sessiont::~api_sessiont() = default;

struct api_messaget
{
  std::string string;
};

const char *api_message_get_string(const api_messaget &message)
{
  return message.string.c_str();
}

class api_message_handlert : public message_handlert
{
public:
  explicit api_message_handlert(
    api_message_callbackt callback,
    api_call_back_contextt context);

  void print(unsigned level, const std::string &message) override;

  // Unimplemented for now. We may need to implement these as string conversions
  // or something later.
  void print(unsigned level, const xmlt &xml) override{};
  void print(unsigned level, const jsont &json) override{};

  void flush(unsigned) override{};

private:
  api_call_back_contextt context;
  api_message_callbackt callback;
};

api_message_handlert::api_message_handlert(
  api_message_callbackt callback,
  api_call_back_contextt context)
  : message_handlert{}, context{context}, callback{callback}
{
}

void api_message_handlert::print(unsigned level, const std::string &message)
{
  if(!callback)
    return;
  api_messaget api_message{message};
  callback(api_message, context);
}

void api_sessiont::set_message_callback(
  api_message_callbackt callback,
  api_call_back_contextt context)
{
  implementation->message_handler =
    util_make_unique<api_message_handlert>(callback, context);
}

void api_sessiont::load_model_from_files(
  const std::vector<std::string> &files) const
{
  implementation->model = util_make_unique<goto_modelt>(initialize_goto_model(
    files, *implementation->message_handler, *implementation->options));
}

std::unique_ptr<verification_resultt> api_sessiont::verify_model() const
{
  PRECONDITION(implementation->model);

  // Remove inline assembler; this needs to happen before adding the library.
  remove_asm(*implementation->model);

  // add the library
  messaget log{*implementation->message_handler};
  log.status() << "Adding CPROVER library (" << config.ansi_c.arch << ")"
               << messaget::eom;
  link_to_library(
    *implementation->model,
    *implementation->message_handler,
    cprover_c_library_factory);

  // Common removal of types and complex constructs
  if(::process_goto_program(
       *implementation->model, *implementation->options, log))
  {
    return {};
  }

  // add failed symbols
  // needs to be done before pointer analysis
  add_failed_symbols(implementation->model->symbol_table);

  // label the assertions
  // This must be done after adding assertions and
  // before using the argument of the "property" option.
  // Do not re-label after using the property slicer because
  // this would cause the property identifiers to change.
  label_properties(*implementation->model);

  remove_skip(*implementation->model);

  ui_message_handlert ui_message_handler(*implementation->message_handler);
  all_properties_verifier_with_trace_storaget<multi_path_symex_checkert>
    verifier(
      *implementation->options, ui_message_handler, *implementation->model);
  verification_resultt result;
  auto results = verifier();
  // Print messages collected to callback buffer.
  verifier.report();
  // Set results object before returning.
  result.set_result(results);
  auto properties = verifier.get_properties();
  result.set_properties(properties);
  return util_make_unique<verification_resultt>(result);
}

void api_sessiont::drop_unused_functions() const
{
  INVARIANT(
    implementation->model != nullptr,
    "Model has not been loaded. Load it first using "
    "api::load_model_from_files");

  messaget log{*implementation->message_handler};
  log.status() << "Performing instrumentation pass: dropping unused functions"
               << messaget::eom;

  remove_unused_functions(
    *implementation->model, *implementation->message_handler);
}

void api_sessiont::validate_goto_model() const
{
  INVARIANT(
    implementation->model != nullptr,
    "Model has not been loaded. Load it first using "
    "api::load_model_from_files");

  messaget log{*implementation->message_handler};
  log.status() << "Validating consistency of goto-model supplied to API session"
               << messaget::eom;

  implementation->model->validate();
}


struct api_options_implementationt
{
  // Options for the verification engine
  bool simplify_enabled;

  // Option for dropping unused function
  bool drop_unused_functions_enabled;

  // Option for validating the goto model
  bool validate_goto_model_enabled;
};

api_optionst::api_optionst(
  std::unique_ptr<const api_options_implementationt> implementation)
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

static std::unique_ptr<optionst> make_internal_default_options()
{
  std::unique_ptr<optionst> options = util_make_unique<optionst>();
  cmdlinet command_line;
  PARSE_OPTIONS_GOTO_CHECK(command_line, (*options));
  parse_solver_options(command_line, *options);
  options->set_option("built-in-assertions", true);
  options->set_option("arrays-uf", "auto");
  options->set_option("depth", UINT32_MAX);
  options->set_option("sat-preprocessor", true);
  return options;
}

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
  auto impl = util_make_unique<api_options_implementationt>(*implementation);
  api_optionst api_options{std::move(impl)};
  return api_options;
}

api_optionst::buildert::buildert() = default;
api_optionst::buildert::buildert(api_optionst::buildert &&builder) noexcept = default;
api_optionst::buildert::~buildert() = default;

static std::unique_ptr<optionst> to_engine_options(const api_optionst &api_options)
{
  auto engine_options = make_internal_default_options();
  engine_options->set_option("simplify", api_options.simplify());
  return engine_options;
}
