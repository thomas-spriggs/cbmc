// Author: Fotis Koutoulakis for Diffblue Ltd.

#ifndef CPROVER_LIBCPROVER_CPP_OPTIONS_H
#define CPROVER_LIBCPROVER_CPP_OPTIONS_H

#include <memory>

struct api_options_implementationt;

class api_optionst final
{
private:
 std::unique_ptr<const api_options_implementationt> implementation;
  // Construction is allowed only through the builder.
  explicit api_optionst(std::unique_ptr<const api_options_implementationt> implementation);

public:

  class buildert final
  {
  private:
    std::unique_ptr<api_options_implementationt> implementation;

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

#endif
