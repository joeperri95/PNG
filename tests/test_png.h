#ifndef __TEST_PNG_H__
#define __TEST_PNG_H__

#include "test.h"
#include "png.h"
#include "bitstream.h"

#define NUM_TESTS_PNG 0


bool test_valid_png();
bool test_invalid_png();
bool test_read_header();
bool test_get_unknown_chunk();
bool test_get_chunk_length();
bool test_validate_crc32();
bool test_get_chunk_type();
bool test_chunk_order();
bool test_process_chunk();

#endif
