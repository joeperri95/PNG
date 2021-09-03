#ifndef __TEST_BITSTREAM_H__
#define __TEST_BITSTREAM_H__

#include "bitstream.h"
#include "test.h"

#define NUM_TESTS_BITSTREAM 13

bool test_create_bitstream_length();
bool test_create_bitstream_bit_offset();
bool test_create_bitstream_byte_offset();
bool test_delete_bitstream();
bool test_read_bit();
bool test_read_byte();
bool test_byte_offset_rollover();
bool test_bit_offset_rollover();
bool test_read_byte_rollover();
bool test_read_max_bits();
bool test_read_no_bits();
bool test_read_no_bits_offset();
bool test_read_bit7();




#endif
