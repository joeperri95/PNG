#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

// checksum.h
/*
    A module for the checksums needed by both PNG and GZIP applications
*/

// Includes
#include <stdint.h>
#include "logging.h"

// Public Functions

/* 
    Perform the CRC32 algorithm that is used after each PNG chunk as well as in gzip.
    Also has a number of other uses in many other domains.
    Returns the CRC32 of the buffer which is length bytes long
*/
uint32_t CRC32(uint8_t *buffer, uint32_t length);

/* 
    Perform the CRC16 algorithm required by the gzip header
    Not yet implemented
*/
uint32_t CRC16(uint8_t *buffer, uint32_t len);

/* 
    Perform the adler32 checksum that is used in zlib streams 
    Returns the ADLER32 of the buffer which is length bytes long
*/
uint32_t ADLER32(uint8_t* buffer, uint32_t length);


#endif
