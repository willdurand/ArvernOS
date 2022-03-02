#include <kshell/kshell.h>

#include <arvern/utils.h>
#include <proc/task.h>

void ps(int argc, char* argv[])
{
  UNUSED(argc);
  UNUSED(argv);

  task_print_info();
}
