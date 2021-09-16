#ifndef __TEST_CHECKSUM_H__
#define __TEST_CHECKSUM_H__

#include "checksum.h"
#include "test.h"


#define NUM_TESTS_CHECKSUM 1

bool test_CRC32();
bool test_CRC16();

bool test_ADLER32_short();
bool test_ADLER32_long();

#endif
