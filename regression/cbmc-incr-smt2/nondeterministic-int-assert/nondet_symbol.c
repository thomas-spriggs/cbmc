extern int john();

int main()
{
  int nondet1 = john();
  __CPROVER_assume(nondet1 == 10);
  __CPROVER_assert(nondet1 < 11, "");
  __CPROVER_assert(john() == 0, "");

  __CPROVER_assert(john() == john(), "");
}
