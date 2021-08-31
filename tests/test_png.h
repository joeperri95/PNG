#ifndef __TEST_PNG_H__
#define __TEST_PNG_H__

#include "test.h"
#include "png.h"
#include "bitstream.h"

#define NUM_TESTS_PNG 3

bool test_CRC32();
bool test_CRC32_IHDR();
bool test_SimpleCRC();
/*
static test test_list[] = 
{
    {"test CRC32", test_CRC32},
    {"test CRC32 on IHDR", test_CRC32_IHDR},
    {"test Simple CRC algorithm", test_SimpleCRC},
};

extern test_suite png_suite =
{
    .suite_name = "PNG suite",
    .number_of_tests = NUM_TESTS_PNG,
    .test_list = test_list,
};
*/
#endif
