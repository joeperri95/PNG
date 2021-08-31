#include "test_bitstream.h"

#define TEST_DATA_LENGTH 10
uint8_t data[] = {0x7F, 0xF3, 0xA5, 0x55, 0xAA, 0x99, 0x73, 0x01, 0x00, 0xF7};

bool test_create_bitstream_length()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    ret = assert(bits.length == TEST_DATA_LENGTH);    

    delete_bitstream(&bits);
    return ret;
}   

bool test_create_bitstream_bit_offset()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    ret = assert(bits.bit_offset == 0);    

    delete_bitstream(&bits);
    return ret;
}   

bool test_create_bitstream_byte_offset()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    ret = assert(bits.byte_offset == 0);    

    delete_bitstream(&bits);
    return ret;
}

bool test_delete_bitstream()
{
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);
    delete_bitstream(&bits);

    return assert(bits.buffer == NULL);
}

bool test_read_bit()
{
    bool ret = false;
    bitstream_t bits;
    uint8_t test_data[1] = {0x01};

    create_bitstream(&bits, data, 1);
    ret = assert(read_bits(&bits, 1) == 0x01);
    
    delete_bitstream(&bits);
    return ret;
}

bool test_read_byte()
{

    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    ret = assert(read_bits(&bits, 8) == 0x7F);

    delete_bitstream(&bits);
    return ret;
}

bool test_byte_offset_rollover()
{

    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 8);

    ret = assert(bits.byte_offset == 1);

    delete_bitstream(&bits);
    return ret;
}

bool test_bit_offset_rollover()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 8);

    ret = assert(bits.bit_offset == 0);

    delete_bitstream(&bits);
    return ret;
}

bool test_read_byte_rollover()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    read_bits(&bits, 4);
    ret = assert(read_bits(&bits, 8) == 0x37);

    delete_bitstream(&bits);
    return ret;
}

bool test_read_max_bits()
{
    bool ret = false;
    bitstream_t bits;
    create_bitstream(&bits, data, TEST_DATA_LENGTH);

    uint64_t value = read_bits(&bits, 64);
    ret = assert(value == 0x7FF3A555AA997301);

    delete_bitstream(&bits);
    return ret;
}

static test test_list[NUM_TESTS_BITSTREAM] = 
{
{"test create bitstream length", test_create_bitstream_length},
{"test create bitstream bit offset", test_create_bitstream_bit_offset}, 
{"test create bitstream byte offset", test_create_bitstream_byte_offset}, 
{"test delete bitstream", test_delete_bitstream}, 
{"test read bit", test_read_bit}, 
{"test read byte", test_read_byte}, 
{"test read byte rollover", test_read_byte_rollover}, 
{"test byte offset rollover", test_byte_offset_rollover}, 
{"test bit offset rollover", test_bit_offset_rollover}, 
{"test read max bits", test_read_max_bits},
};

extern test_suite bitstream_suite =
{
    .suite_name = "bitstream test suite",
    .number_of_tests = NUM_TESTS_BITSTREAM,
    .test_list = test_list,
};
