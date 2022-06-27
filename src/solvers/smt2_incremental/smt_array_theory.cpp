// Author: Diffblue Ltd.

#include "smt_array_theory.h"

const char *smt_array_theoryt::selectt::identifier()
{
  return "select";
}

smt_sortt smt_array_theoryt::selectt::return_sort(
  const smt_termt &array,
  const smt_termt &index)
{
  return array.get_sort().cast<smt_array_sortt>()->element_sort();
}

void smt_array_theoryt::selectt::validate(
  const smt_termt &array,
  const smt_termt &index)
{
  const auto array_sort = array.get_sort().cast<smt_array_sortt>();
  INVARIANT(array_sort, "\"select\" may only select from an array.");
  INVARIANT(array_sort->index_sort() == index.get_sort(),
            "Sort of arrays index must match the sort of the index supplied.");
}

const char *smt_array_theoryt::storet::identifier()
{
  return "store";
}
