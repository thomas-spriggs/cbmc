int main()
{
  int example_array[10000];
  unsigned int index;
  __CPROVER_assume(index < 1000);
  example_array[index + 20] = 42;
  __CPROVER_assert(example_array[index] == 42, "Array condition");
  __CPROVER_assert(example_array[index] != 42, "Array condition");
}
