// Author: Thomas Spriggs for Diffblue Ltd.

#ifndef CBMC_SOLVER_OPTIONS_H
#define CBMC_SOLVER_OPTIONS_H

#include <memory>
#include <functional>
#include <string>

enum class sat_solver_typest
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

class sat_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit sat_optionst(std::unique_ptr<const implementationt> implementation);

public:

  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    buildert &operator=(buildert && builder) noexcept;
    ~buildert();

    // Options
    buildert &solver(sat_solver_typest solver);
    buildert &external_sat_solver(std::string external_sat_solver);
    buildert &sat_preprocessor(bool sat_preprocessor);
    buildert &dimacs(bool dimacs);

    /// For doing the building when options have been specified.
    sat_optionst build();
  };

  sat_solver_typest solver();
  std::unique_ptr<std::string> external_sat_solver();
  bool sat_preprocessor();
  bool dimacs();

  sat_optionst(sat_optionst && options) noexcept;
  sat_optionst &operator=(sat_optionst && options) noexcept;
  sat_optionst(const sat_optionst & options);
  sat_optionst &operator=(const sat_optionst & options);
  ~sat_optionst();
};

class legacy_smt_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit legacy_smt_optionst(std::unique_ptr<const implementationt> implementation);

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
    buildert &operator=(buildert && builder) noexcept;
    ~buildert();

    // Options
		buildert & solver_specialisation(solvert solver);
		buildert & use_FPA_theory(bool use_enabled);

    /// For doing the building when options have been specified.
    legacy_smt_optionst build();
  };

  solvert solver_specialisation();
  bool use_FPA_theory();

  legacy_smt_optionst(legacy_smt_optionst && options) noexcept;
  legacy_smt_optionst &operator=(legacy_smt_optionst && options) noexcept;
  legacy_smt_optionst(const legacy_smt_optionst & options);
  legacy_smt_optionst &operator=(const legacy_smt_optionst & options);
  ~legacy_smt_optionst();
};

class incremental_smt_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit incremental_smt_optionst(std::unique_ptr<const implementationt> implementation);

public:
  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    buildert &operator=(buildert && builder) noexcept;
    ~buildert();

    // Options
    buildert & solver_path(std::string path);
    buildert & formula_dump_path(std::string path);

    /// For doing the building when options have been specified.
    incremental_smt_optionst build();
  };

  std::string solver_path();
  std::unique_ptr<std::string> formula_dump_path();

  incremental_smt_optionst(incremental_smt_optionst && options) noexcept;
  incremental_smt_optionst &operator=(incremental_smt_optionst &&) noexcept;
  incremental_smt_optionst(const incremental_smt_optionst & options);
  incremental_smt_optionst &operator=(const incremental_smt_optionst &);
  ~incremental_smt_optionst();
};

class solver_optionst
{
private:
  struct implementationt;

 std::unique_ptr<const implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit solver_optionst(std::unique_ptr<const implementationt> implementation);

public:
  enum class solver_typet {
    sat,
    legacy_smt,
    incremental_smt
  };

  class buildert final
  {
  private:
    std::unique_ptr<implementationt> implementation;

  public:
    buildert();
    buildert(buildert && builder) noexcept;
    buildert &operator=(buildert && builder) noexcept;
    ~buildert();

    // Options
    buildert &sat_options(sat_optionst options);
    buildert &legacy_smt_options(legacy_smt_optionst options);
    buildert &incremental_smt_options(incremental_smt_optionst options);

    buildert &outfile(std::string outfile);
    buildert &write_solver_stats_to(std::string filename);
    buildert &beautify(bool on);
    buildert &max_node_refinement(unsigned int max_node_refinement);
    buildert &refine_arrays(bool refine_arrays);
    buildert &refine_arthimetic(bool refine_arthimetic);

    /// For doing the building when options have been specified.
    solver_optionst build();
  };

  std::unique_ptr<std::string> outfile();
  std::unique_ptr<std::string> write_solver_stats_to();
  bool beautify();
  std::unique_ptr<unsigned int> max_node_refinement();
  bool refine_arrays();
  bool refine_arthimetic();

  void visit_solver_specific_options(
    const std::function<void(sat_optionst)> &sat_visitor,
    const std::function<void(legacy_smt_optionst)> &legacy_smt_visitor,
    const std::function<void(incremental_smt_optionst)> &legacy_smt_vistor);

  solver_optionst(solver_optionst && options) noexcept;
  solver_optionst &operator=(solver_optionst &&) noexcept;
  ~solver_optionst();
};

#endif // CBMC_SOLVER_OPTIONS_H
