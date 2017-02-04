#ifndef _TEST_H_
#define _TEST_H_
#include <stdlib.h>
#include <stdio.h>

void test_module_begin(char *);
void test_module_end(char *);

void begin_subtest(char *);
void end_subtest(char *);

void test_fail(int print, char *fmt, ...);
void test_pass(int print, char *fmt, ...);

#define expect_ok_noprint(expected, actual)	\
    do {					\
	if (expected == actual) {		\
	    test_pass(0, "");			\
	} else {				\
	    test_fail(0, "");			\
	}					\
    } while(0)

#define expect_ok(expected, actual)				\
    do {							\
	if (expected != actual) {				\
	    test_fail(1, "FAIL: "#expected" != "#actual"\n");	\
	} else {						\
	    test_pass(1, "PASS\n");				\
	}							\
    } while(0)

#endif
