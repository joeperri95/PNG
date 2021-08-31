#ifndef __TEST_H__
#define __TEST_H__

#include "logging.h"

// a test is a function pointer that takes no args and returns no value
// should contain everything it needs to work and perform 1 assert
typedef bool (*_test)(void);

typedef struct
{
    char *test_name;
    _test procedure;
} test;

// test results contain the number of passes and fails from a suite run
typedef struct
{
    int num_passed;
    int num_failed;
    int num_total;
} test_results;


// a test suite is just a collection of tests to run with a name
typedef struct
{
    char* suite_name;
    int number_of_tests;
    test *test_list;
} test_suite;

// evaluate expression but don't halt the program
#define  assert(expression) (expression) 



#endif
