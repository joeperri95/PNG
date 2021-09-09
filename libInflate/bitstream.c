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
    bits->buffer = NULL;
}

uint8_t _read_bits_big_endian(bitstream_t *bits, int N)
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

        // read lower byte
        ret = (ret << (endbit - 0x08)) + (data & ((mask &0xFF00) >> 0x08 ));
        bits->bit_offset = (endbit) & 0x07;        

    }
    else if(endbit == 8)
    {
        // read current byte
        ret = (data & (mask & 0x00FF)) >> (bits->bit_offset);
        
        bits->byte_offset++;
        bits->bit_offset = 0;
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

uint8_t _read_bits_little_endian(bitstream_t *bits, int N)
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
        ret = (data & (mask & 0x00FF)) >> (bits->bit_offset);
        
        bits->byte_offset++;
        bits->bit_offset = 0;
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

uint64_t read_bits_little_endian(bitstream_t *bits, int N)
{

    if(N > 64)
    {
        LOG(ERROR, "Can only read up to 64 bits\n");
        return 0XFFFFFFFF;
    }
    else if(N == 0)
    {
        return 0;
    }

    int bits_left = N;
    int shift = 0;
    uint64_t ret = 0;

    while(bits_left >= 8)
    {        
        ret += _read_bits_little_endian(bits, 8) << shift;
        
        shift += 8;
        bits_left -= 8;
    }

    ret += _read_bits_little_endian(bits, bits_left) << (shift);

    return ret;
}

uint64_t read_bits_big_endian(bitstream_t *bits, int N)
{

    if(N > 64)
    {
        LOG(ERROR, "Can only read up to 64 bits\n");
        return 0XFFFFFFFF;
    }
    else if(N == 0)
    {
        return 0;
    }

    int bits_left = N;
    uint64_t ret = 0;

    while(bits_left >= 8)
    {
        ret = ret << 0x08;
        ret |= _read_bits_big_endian(bits, 8);
        
        bits_left -= 8;
    }

    ret = ret << bits_left;
    ret += _read_bits_big_endian(bits, bits_left);

    return ret;
}
