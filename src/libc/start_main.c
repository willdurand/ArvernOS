#include <stdio.h>

int start_main(int (*main)(int, char**), int argc, char* argv[])
{
  /*
  printf("start_main: main=%p argc=%d argv=%p argv[0]=%s\n",
         main,
         argc,
         argv,
         argv[0]);
  */

  return main(argc, argv);

  // TODO: call `exit()`
}
