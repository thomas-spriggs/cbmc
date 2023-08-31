#include <stdlib.h>

#ifdef _WIN32
#  include <malloc.h>
#else
#  include <alloca.h>
#endif

int main()
{
  int *p = alloca(sizeof(int));
  *p = 42;
  free(p);
  (void)alloca(sizeof(int)); // useless, but result still needs to be released
}
