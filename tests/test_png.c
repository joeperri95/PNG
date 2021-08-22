#include "test_png.h"
#include "bitstream.h"
#include <stdio.h>
#include <assert.h>


void make_crc_table(void)
{
        
}





void test_CRC32()
{

}

void test_SimpleCRC()
{
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
        printf("%d 0x%lx\n",index++,  remainder);

    }

    remainder = (dividend ^ 0xFFFFFFFF);              

    delete_bitstream(&bits);
}
