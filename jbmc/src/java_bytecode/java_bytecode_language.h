/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#ifndef CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_LANGUAGE_H
#define CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_LANGUAGE_H

#include "assignments_from_json.h"
#include "ci_lazy_methods_needed.h"
#include "ci_lazy_methods_v11.h"
#include "java_class_loader.h"
#include "java_object_factory_parameters.h"
#include "java_static_initializers.h"
#include "java_string_library_preprocess.h"
#include "select_pointer_type.h"
#include "synthetic_methods_map.h"

#include <memory>

#include <util/cmdline.h>
#include <util/make_unique.h>
#include <util/prefix_filter.h>

#include <langapi/language.h>

// clang-format off
#define JAVA_BYTECODE_LANGUAGE_OPTIONS /*NOLINT*/ \
  "(disable-uncaught-exception-check)" \
  "(throw-assertion-error)" \
  "(java-assume-inputs-non-null)" \
  "(java-assume-inputs-interval):" \
  "(java-assume-inputs-integral)" \
  "(throw-runtime-exceptions)" \
  "(max-nondet-array-length):" \
  "(max-nondet-tree-depth):" \
  "(java-max-vla-length):" \
  "(java-cp-include-files):" \
  "(context-include):" \
  "(context-exclude):" \
  "(no-lazy-methods)" \
  "(lazy-methods-extra-entry-point):" \
  "(java-load-class):" \
  "(java-no-load-class):" \
  "(static-values):"

#define JAVA_BYTECODE_LANGUAGE_OPTIONS_HELP /*NOLINT*/ \
  " --disable-uncaught-exception-check\n" \
  "                              ignore uncaught exceptions and errors\n" \
  " --throw-assertion-error      throw java.lang.AssertionError on violated\n" \
  "                              assert statements instead of failing\n" \
  "                              at the location of the assert statement\n" \
  " --throw-runtime-exceptions   make implicit runtime exceptions explicit\n" \
  " --max-nondet-array-length N  limit nondet (e.g. input) array size to <= N\n" /* NOLINT(*) */ \
  " --max-nondet-tree-depth N    limit size of nondet (e.g. input) object tree;\n" /* NOLINT(*) */ \
  "                              at level N references are set to null\n" /* NOLINT(*) */ \
  " --java-assume-inputs-non-null\n" \
  "                              never initialize reference-typed parameter to the\n" /* NOLINT(*) */ \
  "                              entry point with null\n" /* NOLINT(*) */ \
  " --java-assume-inputs-interval [L:U] or [L:] or [:U]\n" \
  "                              force numerical primitive-typed inputs\n" /* NOLINT(*) */ \
  "                              (byte, short, int, long, float, double) to be\n" /* NOLINT(*) */ \
  "                              initialized within the given range; lower bound\n" /* NOLINT(*) */ \
  "                              L and upper bound U must be integers;\n" /* NOLINT(*) */ \
  "                              does not work for arrays;\n" /* NOLINT(*) */ \
  " --java-assume-inputs-integral\n" \
  "                              force float and double inputs to have integer values;\n" /* NOLINT(*) */ \
  "                              does not work for arrays;\n" /* NOLINT(*) */ \
  " --java-max-vla-length N      limit the length of user-code-created arrays\n" /* NOLINT(*) */ \
  " --java-cp-include-files r    regexp or JSON list of files to load\n" \
  "                              (with '@' prefix)\n" \
  " --context-include i          only analyze code matching specification i that\n" /* NOLINT(*) */ \
  " --context-exclude e          does not match specification e.\n" \
  "                              A specification is any prefix of a package, class\n" /* NOLINT(*) */ \
  "                              or method name, e.g. \"org.cprover.\" or\n" /* NOLINT(*) */ \
  "                              \"org.cprover.MyClass.\" or\n" \
  "                              \"org.cprover.MyClass.methodToStub:(I)Z\".\n" \
  "                              These options can be given multiple times.\n" \
  "                              The default for context-include is 'all\n" \
  "                              included'; default for context-exclude is\n" \
  "                              'nothing excluded'.\n" \
  " --no-lazy-methods            load and translate all methods given on\n" \
  "                              the command line and in --classpath\n" \
  "                              Default is to load methods that appear to be\n" /* NOLINT(*) */ \
  "                              reachable from the --function entry point\n" \
  "                              or main class\n" \
  "                              Note that --show-symbol-table, --show-goto-functions\n" /* NOLINT(*) */ \
  "                              and --show-properties output are restricted to\n" /* NOLINT(*) */ \
  "                              loaded methods by default.\n" \
  " --lazy-methods-extra-entry-point METHODNAME\n" \
  "                              treat METHODNAME as a possible program entry\n" /* NOLINT(*) */ \
  "                              point for the purpose of lazy method loading\n" /* NOLINT(*) */ \
  "                              METHODNAME can be a regex that will be matched\n" /* NOLINT(*) */ \
  "                              against all symbols. If missing a java:: prefix\n"    /* NOLINT(*) */ \
  "                              will be added. If no descriptor is found, all\n"/* NOLINT(*) */ \
  "                              overloads of a method will also be added.\n" \
  " --static-values f            Load initial values of static fields from the given\n"/* NOLINT(*) */ \
  "                              JSON file. We assign static fields to these values\n"/* NOLINT(*) */ \
  "                              instead of calling the normal static initializer\n"/* NOLINT(*) */ \
  "                              (clinit) method.\n" \
  "                              The argument can be a relative or absolute path to\n"/* NOLINT(*) */ \
  "                              the file.\n"
// clang-format on

class symbolt;

enum lazy_methods_modet
{
  LAZY_METHODS_MODE_EAGER,
  LAZY_METHODS_MODE_EXTERNAL_DRIVER
};

#define JAVA_CLASS_MODEL_SUFFIX "@class_model"

class java_bytecode_languaget:public languaget
{
public:
  void set_language_options(const optionst &) override;

  virtual bool preprocess(
    std::istream &instream,
    const std::string &path,
    std::ostream &outstream) override;

  bool parse(
    std::istream &instream,
    const std::string &path) override;

  bool generate_support_functions(
    symbol_tablet &symbol_table) override;

  bool typecheck(
    symbol_tablet &context,
    const std::string &module) override;

  virtual bool final(symbol_table_baset &context) override;

  void show_parse(std::ostream &out) override;

  virtual ~java_bytecode_languaget();
  java_bytecode_languaget(
    std::unique_ptr<select_pointer_typet> pointer_type_selector)
    : language_options_initialized(false),
      threading_support(false),
      assume_inputs_non_null(false),
      object_factory_parameters(),
      max_user_array_length(0),
      lazy_methods_mode(lazy_methods_modet::LAZY_METHODS_MODE_EAGER),
      string_refinement_enabled(false),
      throw_runtime_exceptions(false),
      assert_uncaught_exceptions(false),
      throw_assertion_error(false),
      nondet_static(false),
      pointer_type_selector(std::move(pointer_type_selector))
  {
  }

  java_bytecode_languaget():
    java_bytecode_languaget(
      std::unique_ptr<select_pointer_typet>(new select_pointer_typet()))
  {
  }

  bool from_expr(
    const exprt &expr,
    std::string &code,
    const namespacet &ns) override;

  bool from_type(
    const typet &type,
    std::string &code,
    const namespacet &ns) override;

  bool to_expr(
    const std::string &code,
    const std::string &module,
    exprt &expr,
    const namespacet &ns) override;

  std::unique_ptr<languaget> new_language() override
  { return util_make_unique<java_bytecode_languaget>(); }

  std::string id() const override { return "java"; }
  std::string description() const override { return "Java Bytecode"; }
  std::set<std::string> extensions() const override;

  void modules_provided(std::set<std::string> &modules) override;
  virtual void
  methods_provided(std::unordered_set<irep_idt> &methods) const override;
  virtual void convert_lazy_method(
    const irep_idt &function_id,
    symbol_table_baset &symbol_table) override;

protected:
  void convert_single_method(
    const irep_idt &function_id,
    symbol_table_baset &symbol_table)
  {
    convert_single_method(
      function_id, symbol_table, optionalt<ci_lazy_methods_neededt>());
  }
  bool convert_single_method(
    const irep_idt &function_id,
    symbol_table_baset &symbol_table,
    optionalt<ci_lazy_methods_neededt> needed_lazy_methods);

  const select_pointer_typet &get_pointer_type_selector() const;

  bool language_options_initialized;
  irep_idt main_class;
  std::vector<irep_idt> main_jar_classes;
  java_class_loadert java_class_loader;
  bool threading_support;
  bool assume_inputs_non_null;      // assume inputs variables to be non-null
  java_object_factory_parameterst object_factory_parameters;
  size_t max_user_array_length;     // max size for user code created arrays
  method_bytecodet method_bytecode;
  lazy_methods_modet lazy_methods_mode;
  bool string_refinement_enabled;
  bool throw_runtime_exceptions;
  bool assert_uncaught_exceptions;
  bool throw_assertion_error;
  java_string_library_preprocesst string_preprocess;
  std::string java_cp_include_files;
  bool nondet_static;
  /// Path to a JSON file which contains initial values of static fields (right
  /// after the static initializer of the class was run). This is read from the
  /// --static-values command-line option.
  std::string static_values_file;

  // list of classes to force load even without reference from the entry point
  std::vector<irep_idt> java_load_classes;

private:
  virtual std::vector<load_extra_methodst>
  build_extra_entry_points(const optionst &) const;
  const std::unique_ptr<const select_pointer_typet> pointer_type_selector;

  /// Maps synthetic method names on to the particular type of synthetic method
  /// (static initializer, initializer wrapper, etc). For full documentation see
  /// synthetic_method_map.h
  synthetic_methods_mapt synthetic_methods;
  stub_global_initializer_factoryt stub_global_initializer_factory;
  class_hierarchyt class_hierarchy;
  // List of classes to never load
  std::unordered_set<std::string> no_load_classes;

  /// Map used in all calls to functions that deterministically create objects
  /// (currently only \ref assign_from_json).
  /// It tracks objects that should be reference-equal to each other by mapping
  /// IDs of such objects to symbols that store their values.
  std::unordered_map<std::string, object_creation_referencet> references;

  /// If set, method bodies are only elaborated if they pass the filter
  optionalt<prefix_filtert> method_in_context;
};

std::unique_ptr<languaget> new_java_bytecode_language();

void parse_java_language_options(const cmdlinet &cmd, optionst &options);

#endif // CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_LANGUAGE_H
