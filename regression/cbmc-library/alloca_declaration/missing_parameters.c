#include <assert.h>
#include <stddef.h>

void *alloca();

int main()
{
  int n;
  __CPROVER_assume(5 <= n && n < 10);
  int *C = alloca();
  assert(C);
  return 0;
}
