// Author: Fotis Koutoulakis for Diffblue Ltd.

#ifndef CPROVER_LIBCPROVER_CPP_OPTIONS_H
#define CPROVER_LIBCPROVER_CPP_OPTIONS_H

#include <memory>
#include <string>

class api_optionst final
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_optionst(std::unique_ptr<const implementationt> implementation);

public:

  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    ~buildert();

    // Options
    buildert &simplify(bool on);
    buildert &drop_unused_functions(bool on);
    buildert &validate_goto_model(bool on);

    /// For doing the building when options have been specified.
    api_optionst build();
  };

  bool simplify() const;
  bool drop_unused_functions() const;
  bool validate_goto_model() const;

  api_optionst(api_optionst && api_options) noexcept;
  ~api_optionst();
};

enum class api_sat_solvert
{
  zchaff,
  booleforce,
  minisat1,
  minisat2,
  ipasir,
  picosat,
  lingeling,
  glucose,
  cadical
};

class api_sat_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_sat_optionst(std::unique_ptr<const implementationt> implementation);

public:

  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    ~buildert();

    // Options
    void solver(api_sat_solvert solver);
    void external_sat_solver(std::string external_sat_solver);
    void sat_preprocessor(bool sat_preprocessor);
    void dimacs(bool dimacs);

    /// For doing the building when options have been specified.
    api_sat_optionst build();
  };

  api_sat_solvert solver();
  std::unique_ptr<std::string> external_sat_solver();
  bool sat_preprocessor();
  bool dimacs();

  api_sat_optionst(api_sat_optionst && api_options) noexcept;
  ~api_sat_optionst();
};

class api_legacy_smt_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_legacy_smt_optionst(std::unique_ptr<const implementationt> implementation);

public:
  enum class solvert
  {
    GENERIC,
    BITWUZLA,
    BOOLECTOR,
    CPROVER_SMT2,
    CVC3,
    CVC4,
    CVC5,
    MATHSAT,
    YICES,
    Z3
  };

  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    ~buildert();

    // Options
		void solver_specialisation(solvert solver);
		void use_FPA_theory(bool use_enabled);

    /// For doing the building when options have been specified.
    api_legacy_smt_optionst build();
  };

  solvert solver_specialisation();
  bool use_FPA_theory();

  api_legacy_smt_optionst(api_legacy_smt_optionst && api_options) noexcept;
  ~api_legacy_smt_optionst();
};

class api_incremental_smt_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_incremental_smt_optionst(std::unique_ptr<const implementationt> implementation);

public:
  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    ~buildert();

    // Options
    void solver_path(std::string path);

    /// For doing the building when options have been specified.
    api_incremental_smt_optionst build();
  };

  std::string solver_path();

  api_incremental_smt_optionst(api_incremental_smt_optionst && api_options) noexcept;
  ~api_incremental_smt_optionst();
};

class api_solver_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_solver_optionst(std::unique_ptr<const implementationt> implementation);

public:
  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    ~buildert();

    // Options
    void outfile(std::string outfile);
    void write_solver_stats_to(std::string filename);
    void beautify(bool on);
    void max_node_refinement(unsigned int max_node_refinement);
    void refine_arrays(bool refine_arrays);
    void refine_arthimetic(bool refine_arthimetic);

    /// For doing the building when options have been specified.
    api_solver_optionst build();
  };

  std::string outfile;
  std::string write_solver_stats_to;
  bool beautify;
  unsigned int max_node_refinement;
  bool refine_arrays;
  bool refine_arthimetic;

  api_solver_optionst(api_solver_optionst && api_options) noexcept;
  ~api_solver_optionst();
};

#endif
