#include "test.h"
#include "test_bitstream.h"
#include "test_huffman.h"

void main(int argc, char *argv)
{

    test bitstream_suite[NUM_TESTS_BITSTREAM] = {test_create_bitstream_length, test_create_bitstream_bit_offset,
    test_create_bitstream_byte_offset, test_read_bit, test_read_byte, test_read_byte_rollover,
    test_bit_offset_rollover, test_byte_offset_rollover};
    
    for(int i = 0; i < NUM_TESTS_BITSTREAM; i++)
    {
        bitstream_suite[i]();
    }
    
    printf( "All bitstream tests " ANSI_COLOR_GREEN "PASSED\n" ANSI_COLOR_RESET);

    test huffman_suite[NUM_TESTS_HUFFMAN] = {test_create_node_data, test_create_node_left_null,
    test_create_node_right_null, test_insert_left, test_insert_right, test_insert_empty};

    for(int i = 0; i < NUM_TESTS_HUFFMAN; i++)
    {
        huffman_suite[i]();
    }

    printf( "All huffman tests " ANSI_COLOR_GREEN "PASSED\n" ANSI_COLOR_RESET);
    printf( "All tests " ANSI_COLOR_GREEN "PASSED\n" ANSI_COLOR_RESET);

    return 0;

}
