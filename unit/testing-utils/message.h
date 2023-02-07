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

class test_ui_message_handlert_plain : public ui_message_handlert
{
public:
  explicit test_ui_message_handlert_plain(std::ostream &out)
    : ui_message_handlert(cmdlinet(), "")
  {
  }

  uit get_ui() const
  {
    return uit::PLAIN;
  }
};



#endif // CPROVER_TESTING_UTILS_MESSAGE_H
