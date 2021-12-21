#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __is_libk
#include <panic.h>
#endif

#ifndef STACK_CHK_GUARD
#ifdef __arm__
#define STACK_CHK_GUARD 0xbaadc0de
#else
#define STACK_CHK_GUARD 0xbaadc0debaadc0de
#endif
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

/**
 * Basic stack smashing protector implementation
 * Based on https://wiki.osdev.org/Stack_Smashing_Protector
 */
void __stack_chk_fail(void)
{
#ifdef __is_libk
  PANIC_NO_STACKTRACE("Stack smashing detected");
#else
  printf("Stack smashing detected! Aborting program.\n");
  abort();
#endif
}
