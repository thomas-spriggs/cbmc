/*******************************************************************\

Module: Unit test utilities

Author: Diffblue Limited.

\*******************************************************************/

#ifndef CPROVER_TESTING_UTILS_MESSAGE_H
#define CPROVER_TESTING_UTILS_MESSAGE_H

#include <util/cmdline.h>
#include <util/ui_message.h>
#include <util/message.h>
#include <util/json_stream.h>

extern null_message_handlert null_message_handler;


// Used primarily for driving some tests using the JSON ui interface (json_symbol_table.cpp)
// but is of greater generality, added in this file to enable better discovery of the class.
class test_ui_message_handlert : public ui_message_handlert
{
public:
  explicit test_ui_message_handlert(std::ostream &out)
    : ui_message_handlert(cmdlinet(), ""), json_stream_array(out, 0)
  {
  }

  uit get_ui() const
  {
    return uit::JSON_UI;
  }

  json_stream_arrayt &get_json_stream()
  {
    return json_stream_array;
  }

  json_stream_arrayt json_stream_array;
};

// Used for driving components that produce a lot of noisy output which is
// mostly unneeded for testing. An example of this is running the verification
// engine in run_verification_engine.cpp
class test_null_ui_message_handler : public ui_message_handlert
{
  public:
  explicit test_null_ui_message_handler(std::ostream &out)
    : ui_message_handlert(cmdlinet(), "unit")
  {
  }

  uit get_ui() const override
  {
    return uit::PLAIN;
  }

  void print(
    unsigned level,
    const std::string &message,
    const source_locationt &location) override {}
};

#endif // CPROVER_TESTING_UTILS_MESSAGE_H
