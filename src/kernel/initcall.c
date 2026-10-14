#include <initcall.h>

#include <logging.h>
#include <panic.h>

void initcall_init()
{
  DEBUG("initcalls: __initcall_start=%p __initcall_end=%p",
        (void*)&__initcall_start,
        (void*)&__initcall_end);

  initcall_foreach(call_fn)
  {
    int retval = (*call_fn)();

    if (retval != 0) {
      PANIC("initcalls: call_fn=%p failed with retval=%d", *call_fn, retval);
    }
  }
}
