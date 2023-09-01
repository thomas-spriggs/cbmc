#include <assert.h>
#include <stddef.h>

void *alloca(size_t a, size_t b);

int main()
{
  int n;
  __CPROVER_assume(5 <= n && n < 10);
  int *C = alloca(n * sizeof(int), 42);
  assert(C);
  return 0;
}
