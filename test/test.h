#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdbool.h>

int assertions = 0;
int failures = 0;

void assert_impl(bool expr, const char* func, const char* message) {
    printf("it %-66s [%s]\n", message, expr ? "pass" : "fail");
    assertions++;

    if (!expr) {
        failures++;
    }
}

int summary_impl(const char* name) {
    printf("\n=> %-48s failures: %2d / total: %2d\n", name, failures, assertions);

    return failures == 0 ? 0 : 1;
}

#define assert(expr, message)   assert_impl((expr), __func__, message)
#define test_summary()          summary_impl(__FILE__)

#endif
