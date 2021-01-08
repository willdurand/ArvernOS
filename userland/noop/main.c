#include <unistd.h>

int main()
{
  while (1) {
    yield();
  }

  return 0;
}
