#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdio.h>

#define RED   "\033[0;31m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

int assertions = 0;
int failures = 0;
bool has_describe = 0;

void _assert(bool expr, const char* func, const char* message)
{
  printf(expr ? CYAN : RED);

  if (has_describe) {
    printf("  it %-64s [%s]\n", message, expr ? "pass" : "fail");
  } else {
    printf("it %-66s [%s]\n", message, expr ? "pass" : "fail");
  }

  printf(RESET);

  assertions++;

  if (!expr) {
    failures++;
  }
}

void _describe(const char* message)
{
  printf(CYAN);
  printf("\n%s\n", message);
  printf(RESET);
  has_describe = true;
}

void _end_describe()
{
  has_describe = false;
}

int _test_summary(const char* name)
{
  printf(failures == 0 ? CYAN : RED);
  printf("\n=> %-48s failures: %2d / total: %2d\n", name, failures, assertions);
  printf(RESET);

  return failures == 0 ? 0 : 1;
}

#define assert(expr, message) _assert((expr), __func__, message)
#define describe(message)     _describe(message)
#define end_describe()        _end_describe()
#define test_summary()        _test_summary(__FILE__)

#endif
