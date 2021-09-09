#include "test_checksum.h"

bool test_ADLER32()
{
    char *test_data = "123456789";
    return assert(ADLER32(test_data, 9) == 0x091E01DE);
}

static test test_list[NUM_TESTS_CHECKSUM] =
{
    {"ADLER32 test", test_ADLER32},
};

extern test_suite inflate_suite =
{
    .number_of_tests = NUM_TESTS_CHECKSUM,
    .suite_name = "Inflate suite",
    .test_list = test_list,
};
