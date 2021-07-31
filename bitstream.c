#include "bitstream.h"

void create_bitstream(bitstream_t *bits, uint8_t *data, uint32_t length)
{

    bits->bit_offset = 0,
    bits->byte_offset = 0,
    bits->length = length,
    bits->buffer = (uint8_t *) malloc(length * sizeof(uint8_t)),
    
    memcpy(bits->buffer, data, length);
}

void delete_bitstream(bitstream_t *bits)
{
    free(bits->buffer);
}

uint8_t read_bits(bitstream_t *bits, int N)
{
    
    uint16_t mask = 0x0000;
    uint16_t endbit = bits->bit_offset + N;
    uint8_t data = *(bits->buffer + bits->byte_offset);
    uint8_t ret = 0;


    if(N == 0){
        return 0;
    }

    // construct mask
    mask = ((0x01 << endbit) - 1) - ((0x01 << bits->bit_offset) - 1);    

    if(endbit > 8){
    // byte wrap

        // read current byte
        ret = (data & (mask &0x00FF)) >> (bits->bit_offset);
        
        bits->byte_offset++;
        if(bits->byte_offset >= bits->length){
            return ret;
        }

        data = *(bits->buffer + bits->byte_offset);

        // read upper byte
        ret += (data & ((mask &0xFF00) >> 0x08 )) << (0x08 - bits->bit_offset);
        bits->bit_offset = (endbit) & 0x07;        

    }
    else if(endbit == 8)
    {
        // read current byte
        ret = (data & (mask &0x00FF)) >> (bits->bit_offset);

        
        bits->byte_offset++;
        bits->bit_offset = 0;

        if(bits->byte_offset > bits->length){
            return 0;
        }

        data = *(bits->buffer + bits->byte_offset);
        
    }

    else
    // code has not rolled over
    {			
        
        ret = (data & mask) >> bits->bit_offset;	
        bits->bit_offset = (bits->bit_offset + N) & 0x07;        

    }

    LOG(10, "bits read %d\n", bits->bit_offset + bits->byte_offset * 8);
    return ret;

}

