#include <stdio.h>

int main(int argc, char* argv[])
{
  printf("%s started", argv[0]);

  if (argc == 1) {
    printf(" without any argument\n");
  } else {
    printf(" with the following arguments:\n");

    for (int i = 1; i < argc; i++) {
      printf("  %02d: %s\n", i, argv[i]);
    }
  }

  while (1) {
    ;
  }

  return 0;
}
