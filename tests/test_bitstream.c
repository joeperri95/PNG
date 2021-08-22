#include "bitstream.h"
#include "logging.h"
#include "test.h"
#include <assert.h>

#define TEST_DATA_LENGTH 10
uint8_t data[] = {0x7F, 0xF3, 0xA5, 0x55, 0xAA, 0x99, 0x73, 0x01, 0x00, 0xF7};

void test_create_bitstream_length()
{
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    assert(bits.length == TEST_DATA_LENGTH);    

    delete_bitstream(&bits);
}   

void test_create_bitstream_bit_offset()
{
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    assert(bits.bit_offset == 0);    

    delete_bitstream(&bits);
}   

void test_create_bitstream_byte_offset()
{
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    assert(bits.byte_offset == 0);    

    delete_bitstream(&bits);
}

void test_delete_bitstream()
{

    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);
    delete_bitstream(&bits);

    assert(bits.buffer == NULL);
}

void test_read_bit()
{
    bitstream_t bits;
    uint8_t test_data[1] = {0x01};

    create_bitstream(&bits, data, 1);
    assert(read_bits(&bits, 1) == 0x01);
    
    delete_bitstream(&bits);
}

void test_read_byte()
{

    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    assert(read_bits(&bits, 8) == 0x7F);

    delete_bitstream(&bits);
}

void test_byte_offset_rollover()
{

    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 8);

    assert(bits.byte_offset == 1);

    delete_bitstream(&bits);
}

void test_bit_offset_rollover()
{

    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 8);

    assert(bits.bit_offset == 0);

    delete_bitstream(&bits);
}

void test_read_byte_rollover()
{
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 4);
    assert(read_bits(&bits, 8) == 0x37);

    delete_bitstream(&bits);
}
