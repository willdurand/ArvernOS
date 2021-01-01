#include <parg.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  printf("init: %s started with pid=%d\n", argv[0], getpid());

  struct parg_state ps;
  parg_init(&ps);

  int c;
  while ((c = parg_getopt(&ps, argc, argv, "h")) != -1) {
    switch (c) {
      case 'h':
        printf("USAGE:\n  init [-h]\n");
        return 0;
    }
  }

  const char* path = "/bin/shell";
  char* args[] = { (char*)path, NULL };

  printf("init: executing %s\n", path);

  return execv(path, args);

  return 0;
}
