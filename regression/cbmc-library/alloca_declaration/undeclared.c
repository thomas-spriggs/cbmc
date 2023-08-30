#include <assert.h>

int main()
{
  int n;
  __CPROVER_assume(5 <= n && n < 10);
  int *C = alloca(n * sizeof(int));
  assert(C);
  return 0;
}
