#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int subtest_ok = 0;
int subtest_ntests = 0;
int test_module_ntests = 0;
int test_module_ok = 0;
char *subtestname = NULL;
char *test_module = NULL;

#define MODULE_BOUND "===================================\n"
#define SUBTEST_BOUND "------------------------------------\n"
void test_module_begin(char *modname)
{
    if (test_module != NULL) {
	fprintf(stderr, "WARNING: you're already in test module %s\n", test_module);
    }
    printf("begin test module %s\n", modname);
    test_module = modname;
}

void test_module_end(char *modname)
{
    if (strcmp(modname, test_module)) {
	fprintf(stderr, "WARNING: %s is not the current module\n", modname);
    }
    printf(MODULE_BOUND);
    printf("end test module %s\n", modname);
    printf("passed: %d\n", test_module_ok);
    printf("failed: %d\n", test_module_ntests - test_module_ok);
    printf("total: %d\n", test_module_ntests);
    printf(MODULE_BOUND);
    test_module = NULL;
}

void begin_subtest(char *cur_testname)
{
    printf("begin subtest %s\n", cur_testname);
    subtestname = cur_testname;
    subtest_ntests = subtest_ok = 0;
}

void end_subtest(char *testname)
{
    if (!testname) {
	fprintf(stderr, "WARNING: no sub-test name currently defined\n");
    }
    printf(SUBTEST_BOUND);
    printf("end subtest %s\n", testname);
    printf("passed %d\n",  subtest_ok);
    printf("failed %d\n", subtest_ntests - subtest_ok);
    printf("total %d\n", subtest_ntests);
    printf(SUBTEST_BOUND);
    subtest_ntests = subtest_ok = 0;
    subtestname = NULL;
}

void test_pass(int print, char *fmt, ...)
{
    va_list vargs;
    if (subtestname) {
	subtest_ntests++;
	subtest_ok++;
    }

    test_module_ntests++;
    test_module_ok++;
    if (print)  {
	va_start(vargs, fmt);
	vfprintf(stdout, fmt, vargs);
	va_end(vargs);
    }
}

void test_fail(int print, char *fmt, ...)
{
    va_list vargs;
    
    if (subtestname) {
	subtest_ntests++;
    }
    test_module_ntests++;
    if (print) {
	va_start(vargs, fmt);
	vfprintf(stdout, fmt, vargs);
	va_end(vargs);
    }
}
