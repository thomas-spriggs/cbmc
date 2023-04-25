// Author: Fotis Koutoulakis for Diffblue Ltd.

#ifndef CPROVER_LIBCPROVER_CPP_API_H
#define CPROVER_LIBCPROVER_CPP_API_H

#include <memory>
#include <string>
#include <vector>

// Forward declaration of API dependencies
struct api_session_implementationt;
struct api_options_implementationt;
class api_optionst;

// There has been a design decision to allow users to include all of
// the API headers by just including `api.h`, so we want to have an
// include for all the API headers below. If we get any auxiliary
// development tools complaining about the includes, please use
// a pragma like below to silence the warning (at least as long
// as the design principle is to be followed.)

#include "verification_result.h" // IWYU pragma: keep

/// Opaque message type. Properties of messages to be fetched through further
/// api calls.
struct api_messaget;

/// Given a \p api_message, this function returns that message expressed as a
/// C language string.
/// \note The memory for the returned string is owned by the message and as such
/// does not need to be freed by users of the API.
const char *api_message_get_string(const api_messaget &message);

/// The type of pointers to contextual data passed to the api_message_callback
/// functions. These pointers point to api consumer data and are just passed
/// through to the callback verbatim. These support users of the api to avoid
/// using global variables.
using api_call_back_contextt = void *;

/// The type of call back for feedback of status information and results.
/// \param message: A structured message object. The lifetime of this object is
///   the duration of the call to the callback. So if any data from it is
///   required to persist, then this data must be copied into the API consumers
///   memory.
/// \param call_back_context: A pointer to the context for the function. This
///   is passed through the API to the function and is for use like a capture
///   group. Memory for this object is owned by the consumer of the API.
using api_message_callbackt = void (*)(
  const api_messaget &message,
  api_call_back_contextt call_back_context);

// An object in the pattern of Session Facade - owning all of the memory
// the API is using and being responsible for the management of that.
struct api_sessiont
{
  // Initialising constructor
  api_sessiont();
  explicit api_sessiont(const api_optionst &options);
  ~api_sessiont(); // default constructed in the .cpp file

  /// \param callback: A call back function to receive progress updates and
  ///                  success/failure statuses.
  /// \param context: A context pointer passed through to the callback function.
  ///                 This is used similarly to a capture in a lambda function.
  void set_message_callback(
    api_message_callbackt callback,
    api_call_back_contextt context);

  /// Load a goto_model from a given vector of filenames.
  /// \param files: A vector<string> containing the filenames to be loaded
  void load_model_from_files(const std::vector<std::string> &files) const;

  // Run the verification engine against previously loaded model and return
  // results object pointer.
  std::unique_ptr<verification_resultt> verify_model() const;

  /// Drop unused functions from the loaded goto_model simplifying it
  void drop_unused_functions() const;

  /// Validate the loaded goto model
  void validate_goto_model() const;

  // A simple API version information function.
  std::unique_ptr<std::string> get_api_version() const;

private:
  std::unique_ptr<api_session_implementationt> implementation;
};

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

  api_optionst(api_optionst && api_options);
  ~api_optionst();
};

#endif
