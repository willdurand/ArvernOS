#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  printf("init: %s started", argv[0]);

  if (argc == 1) {
    printf(" without any argument\n");
  } else {
    printf(" with the following arguments:\n");

    for (int i = 1; i < argc; i++) {
      printf("  %02d: %s\n", i, argv[i]);
    }
  }

  const char* path = "/bin/shell";
  char* args[] = { path, NULL };

  printf("init: executing %s\n", path);
  execv(path, args);

  return 0;
}
