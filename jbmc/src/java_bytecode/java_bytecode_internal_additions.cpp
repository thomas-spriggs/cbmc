/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include "java_bytecode_internal_additions.h"

// For INFLIGHT_EXCEPTION_VARIABLE_NAME
#include "java_types.h"
#include "remove_exceptions.h"

#include <linking/static_lifetime_init.h>
#include <util/c_types.h>
#include <util/cprover_prefix.h>
#include <util/std_types.h>

static void create_initialize(symbol_table_baset &symbol_table)
{
  // If __CPROVER_initialize already exists, replace it. It may already exist
  // if a GOTO binary provided it. This behaviour mirrors the ANSI-C frontend.
  symbol_table.remove(INITIALIZE_FUNCTION);

  symbolt initialize;
  initialize.name=INITIALIZE_FUNCTION;
  initialize.base_name=INITIALIZE_FUNCTION;
  initialize.mode=ID_java;

  initialize.type = java_method_typet({}, java_void_type());
  symbol_table.add(initialize);
}

void java_internal_additions(symbol_table_baset &dest)
{
  // add __CPROVER_rounding_mode

  {
    symbolt symbol;
    symbol.base_name = CPROVER_PREFIX "rounding_mode";
    symbol.name=CPROVER_PREFIX "rounding_mode";
    symbol.type=signed_int_type();
    symbol.mode=ID_C;
    symbol.is_lvalue=true;
    symbol.is_state_var=true;
    symbol.is_thread_local=true;
    symbol.is_static_lifetime = true;
    dest.add(symbol);
  }

  // add __CPROVER_malloc_object

  {
    symbolt symbol;
    symbol.base_name = CPROVER_PREFIX "malloc_object";
    symbol.name=CPROVER_PREFIX "malloc_object";
    symbol.type = pointer_type(java_void_type());
    symbol.mode=ID_C;
    symbol.is_lvalue=true;
    symbol.is_state_var=true;
    symbol.is_thread_local=true;
    dest.add(symbol);
  }

  {
    auxiliary_symbolt symbol;
    symbol.base_name = INFLIGHT_EXCEPTION_VARIABLE_BASENAME;
    symbol.name = INFLIGHT_EXCEPTION_VARIABLE_NAME;
    symbol.mode = ID_java;
    symbol.type = pointer_type(java_void_type());
    symbol.type.set(ID_C_no_nondet_initialization, true);
    symbol.value = null_pointer_exprt(to_pointer_type(symbol.type));
    symbol.is_file_local = false;
    symbol.is_static_lifetime = true;
    dest.add(symbol);
  }

  create_initialize(dest);
}
