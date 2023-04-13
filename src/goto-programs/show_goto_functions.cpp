/*******************************************************************\

Module: Show goto functions

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Show goto functions

#include "show_goto_functions.h"

#include <util/ui_message.h>

#include "goto_model.h"
#include "show_goto_functions_json.h"
#include "show_goto_functions_xml.h"

void show_goto_functions(
  const namespacet &ns,
  ui_message_handlert &ui_message_handler,
  const goto_functionst &goto_functions,
  bool list_only)
{
  ui_message_handlert::uit ui = ui_message_handler.get_ui();
  messaget msg(ui_message_handler);
  switch(ui)
  {
  case ui_message_handlert::uit::XML_UI:
  {
    show_goto_functions_xmlt xml_show_functions(list_only);
    msg.status() << xml_show_functions.convert(goto_functions);
  }
  break;

  case ui_message_handlert::uit::JSON_UI:
  {
    show_goto_functions_jsont json_show_functions(list_only);
    msg.status() << json_show_functions.convert(goto_functions);
  }
  break;

  case ui_message_handlert::uit::PLAIN:
    {
      // sort alphabetically
      const auto sorted = goto_functions.sorted();

      for(const auto &fun : sorted)
      {
        const symbolt &symbol = ns.lookup(fun->first);
        const bool has_body = fun->second.body_available();

        if(list_only)
        {
          msg.status() << '\n'
                       << symbol.display_name() << " /* " << symbol.name
                       << (has_body ? "" : ", body not available") << " */";
          msg.status() << messaget::eom;
        }
        else if(has_body)
        {
          msg.status() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";

          msg.status() << messaget::bold << symbol.display_name()
                       << messaget::reset << " /* " << symbol.name << " */\n";
          fun->second.body.output(msg.status());
          fun->second.history.output(msg.status());
          msg.status() << messaget::eom;
        }
      }
    }

    break;
  }
}

void show_goto_functions(
  const goto_modelt &goto_model,
  ui_message_handlert &ui_message_handler,
  bool list_only)
{
  const namespacet ns(goto_model.symbol_table);
  show_goto_functions(
    ns, ui_message_handler, goto_model.goto_functions, list_only);
}
