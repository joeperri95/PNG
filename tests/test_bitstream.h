#ifndef __TEST_BITSTREAM_H__
#define __TEST_BITSTREAM_H__

#define NUM_TESTS_BITSTREAM 9

void test_create_bitstream_length();
void test_create_bitstream_bit_offset();
void test_create_bitstream_byte_offset();
void test_delete_bitstream();
void test_read_bit();
void test_read_byte();
void test_byte_offset_rollover();
void test_bit_offset_rollover();
void test_read_byte_rollover();

#endif
