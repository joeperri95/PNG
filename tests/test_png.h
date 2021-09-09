#ifndef __TEST_PNG_H__
#define __TEST_PNG_H__

#include "test.h"
#include "png.h"
#include "bitstream.h"

#define NUM_TESTS_PNG 0

// these are checksum tests move them
bool test_CRC32();
bool test_CRC32_IHDR();
bool test_SimpleCRC();

#endif
