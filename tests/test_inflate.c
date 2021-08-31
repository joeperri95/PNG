#include "test_inflate.h"

bool test_ADLER32()
{
    char *test_data = "123456789";
    return assert(z_ADLER32(test_data, 9) == 0x091E01DE);
}

static test test_list[NUM_TESTS_INFLATE] =
{
    {"ADLER32 test", test_ADLER32},
};

extern test_suite inflate_suite =
{
    .number_of_tests = NUM_TESTS_INFLATE,
    .suite_name = "Inflate suite",
    .test_list = test_list,
};
