#ifndef __TEST_INFLATE_H__
#define __TEST_INFLATE_H__

#include "test.h"
#include "inflate.h"

#define NUM_TESTS_INFLATE 0

bool test_read_zlib_header();
bool test_read_adler32();
bool test_inflate_single_block();
bool test_inflate_multiple_blocks();
bool test_inflate_invalid_compression();
bool test_inflate_no_compression();
bool test_inflate_fixed_huffman();
bool test_inflate_dynamic_huffman();
#endif
