#include <assert.h>
#include <stddef.h>

char alloca(size_t alloca_size);

int main()
{
  int n;
  __CPROVER_assume(5 <= n && n < 10);
  int *C = alloca(n * sizeof(int));
  assert(C);
  return 0;
}
