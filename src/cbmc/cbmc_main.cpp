/*******************************************************************\

Module: CBMC Main Module

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// CBMC Main Module

/*

  CBMC
  Bounded Model Checking for ANSI-C
  Copyright (C) 2001-2014 Daniel Kroening <kroening@kroening.com>

*/

#include "cbmc_parse_options.h"

#ifdef _MSC_VER
#  include <util/unicode.h>
#endif

#ifdef IREP_HASH_STATS
#include <iostream>
#endif

#ifdef IREP_HASH_STATS
extern unsigned long long irep_hash_cnt;
extern unsigned long long irep_cmp_cnt;
extern unsigned long long irep_cmp_ne_cnt;
#endif

#include <iostream>
#define SHOW_SIZE_ALIGN(x) \
  std::cout << "Size of \"" << #x << "\" is " << sizeof(x) << \
  "\nAlign of \"" << #x << "\" is " << alignof(x) << std::endl;

#ifdef _MSC_VER
int wmain(int argc, const wchar_t **argv_wide)
{
  auto vec=narrow_argv(argc, argv_wide);
  auto narrow=to_c_str_array(std::begin(vec), std::end(vec));
  auto argv=narrow.data();
#else
int main(int argc, const char **argv)
{
#endif
  SHOW_SIZE_ALIGN(void *);
  SHOW_SIZE_ALIGN(irep_idt);
  SHOW_SIZE_ALIGN(irept);
  SHOW_SIZE_ALIGN(irept::baset::dt);
  SHOW_SIZE_ALIGN(irept::baset::named_subt);
  SHOW_SIZE_ALIGN(irept::baset::subt);

  cbmc_parse_optionst parse_options(argc, argv);

  int res = parse_options.main();

  #ifdef IREP_HASH_STATS
  std::cout << "IREP_HASH_CNT=" << irep_hash_cnt << '\n';
  std::cout << "IREP_CMP_CNT=" << irep_cmp_cnt << '\n';
  std::cout << "IREP_CMP_NE_CNT=" << irep_cmp_ne_cnt << '\n';
  #endif

  std::cout << "Total detructions " << irept::total_destructions <<
    " leaf destructions " << irept::leaf_destructions << std::endl;
  return res;
}
