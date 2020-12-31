#include <stdio.h>

int start_main(int (*main)(int, char**), int argc, char* argv[])
{
  // printf("start_main: main=%p argc=%d argv=%p\n", main, argc, argv);

  return main(argc, argv);

  // TODO: call `exit()`
}
