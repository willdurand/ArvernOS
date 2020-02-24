#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdio.h>

int assertions = 0;
int failures = 0;
bool has_describe = 0;

void _assert(bool expr, const char* func, const char* message)
{
  if (!expr) {
    printf("\033[0;31m");
  }

  if (has_describe) {
    printf("  it %-64s [%s]\n", message, expr ? "pass" : "fail");
  } else {
    printf("it %-66s [%s]\n", message, expr ? "pass" : "fail");
  }

  if (!expr) {
    printf("\033[0m");
  }

  assertions++;

  if (!expr) {
    failures++;
  }
}

void _describe(const char* message)
{
  printf("\n%s\n", message);
  has_describe = true;
}

void _end_describe()
{
  has_describe = false;
}

int _test_summary(const char* name)
{
  printf("\n=> %-48s failures: %2d / total: %2d\n", name, failures, assertions);

  return failures == 0 ? 0 : 1;
}

#define assert(expr, message) _assert((expr), __func__, message)
#define describe(message)     _describe(message)
#define end_describe()        _end_describe()
#define test_summary()        _test_summary(__FILE__)

#endif
