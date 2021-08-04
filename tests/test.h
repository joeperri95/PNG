#ifndef __TEST_H__
#define __TEST_H__

#include "logging.h"

// a test is a function pointer that takes no args and returns no value
// should contain everything it needs to work and perform 1 assert
typedef void (*test)(void);

// a test suite is just a collection of tests to run with a name
typedef struct
{
    char* suite_name;
    int number_of_tests;
    test *test_list;
} test_suite;

#endif
