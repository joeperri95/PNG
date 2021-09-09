#include "test_png.h"
/*
bool test_CRC32_IHDR()
{
    #define MESSAGE_LEN 17
    char message[MESSAGE_LEN] = {0x49, 0x48, 0x44, 0x52, 0x0, 0x0, 0x0, 0x80, 0x00, 0x00, 0x00, 0x44, 0x08, 0x02, 0x00, 0x00, 0x00};

    uint32_t res = CRC32(message, MESSAGE_LEN);
    
    // known good crc
    return assert(res == 0xc625aa3e);
}


bool test_CRC32()
{
    #define MESSAGE_LEN 9
    char message[MESSAGE_LEN] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    
    uint32_t res = CRC32(message, MESSAGE_LEN);

    // known good crc
    return assert(res == 0xcbf43926);
}

bool test_SimpleCRC()
{
    // Check of understanding of CRC algorithm. CRC32 does not actually work this way

    uint64_t CRC_polynomial =  0x104C11DB7;
    //uint64_t CRC_polynomial =  0x10814141AB;
    uint32_t remainder = 0;
    uint64_t dividend = 0;    
    
    #define MESSAGE_LEN 13
    char message[MESSAGE_LEN] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0};
    

    bitstream_t bits;
    create_bitstream(&bits, message, MESSAGE_LEN);
    
    dividend = 0;
    bool done = false;
    bool doXOR = false;
    int index = 0;

    while(!done)
    {

        if(bits.byte_offset == (bits.length - 1) && bits.bit_offset == 7)
        {
            done = true;
        }

        doXOR = dividend & 0x100000000;
        dividend <<= 0x01;
        dividend |= read_bits(&bits, 1);
        dividend &= 0x1FFFFFFFF;

        if(doXOR)
        {
            dividend = dividend ^ CRC_polynomial;
        }
               
        remainder = (dividend & 0xFFFFFFFF);            
        //printf("%d 0x%lx\n",index++,  remainder);

    }

    remainder = (dividend ^ 0xFFFFFFFF);              

    delete_bitstream(&bits);

    return false;
}

bool test_invalid_png()
{


}


static test test_list[] =
{
    {"test CRC32", test_CRC32},
    {"test CRC32 on IHDR", test_CRC32_IHDR},
    {"test Simple CRC algorithm", test_SimpleCRC},
};

extern test_suite png_suite =
{
    .suite_name = "PNG suite",
    .number_of_tests = NUM_TESTS_PNG,
    .test_list = test_list,
};
*/
