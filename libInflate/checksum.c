#include "checksum.h"

uint32_t CRC16(uint8_t *buffer, uint32_t len)
{
    // NOT yet implemented
}

uint32_t CRC32(uint8_t *buffer, uint32_t len)
{
    // perform a crc32 using a table as outlined in rfc2083 

    uint64_t c;
    uint64_t crc_register;       
    uint64_t crc_table[256];
        
    // make crc table
    for (int i = 0; i < 256; i++) {
        c = i;
        
        for (int j = 0; j < 8; j++) {
            c = (c & 1) ? 0xedb88320L ^ (c >> 1) : c >> 1;
        }

        crc_table[i] = c;
    }

    crc_register = 0xffffffffL;
    
    for (int i = 0; i < len; i++) {
        crc_register = crc_table[(crc_register ^ buffer[i]) & 0xff] ^ (crc_register >> 8);
    }

    return (uint32_t) (crc_register ^ 0xffffffffL);
}

uint32_t ADLER32(uint8_t* buffer, uint32_t length)
{

    uint16_t A = 1;
    uint16_t B = 0;
    uint32_t result;

    for(int i = 0; i < length; i++)
    {
        
        A = (A + *(buffer + i)) % 65521;
        B = (B + A) % 65521;
        
    }

    result = (((uint32_t) B) << 16) + A;
    return result;
    
}
