#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __is_libk

#include <kernel/panic.h>

#endif

#ifndef STACK_CHK_GUARD
#define STACK_CHK_GUARD 0xdeadbeefa55a857
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

/**
 * Basic stack smashing protector implementation
 * Based on https://wiki.osdev.org/Stack_Smashing_Protector
 */
void __stack_chk_fail(void)
{
#ifdef __is_libk
  PANIC("Stack smashing detected");
#else
  printf("Stack smashing detected! Aborting program.\n");
  // TODO: use `abort()`
#endif
}
