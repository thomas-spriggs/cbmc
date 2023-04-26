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
    //void solver(api_sat_solvert solver);

    /// For doing the building when options have been specified.
    api_sat_optionst build();
  };


  api_sat_optionst(api_sat_optionst && api_options) noexcept;
  ~api_sat_optionst();
};

#endif
