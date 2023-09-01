#include <assert.h>
#include <stddef.h>

void *alloca(char);

int main()
{
  int n;
  __CPROVER_assume(5 <= n && n < 10);
  int *C = alloca(n * sizeof(int));
  assert(C);
  return 0;
}
