#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

// bitstream.h
/*
	The bitsream module is simply a convenience for sequentially reading bits from a buffer and handling overflow.
	Zlib and deflate streams can be treated as a sequence of bits but they are of course stored as bytes.
	It is also useful to be able to read from the bitstream in both a little endian 
	(LSB first) and big endian (MSB first) manner without having to alter the bitsream object itself.
*/

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"

// Constants

// Data structures

/* Bitstream object containing a data buffer and the current byte and bit offset */
typedef struct {
	uint32_t length;
	uint32_t byte_offset;
	uint8_t *buffer;
	uint8_t bit_offset;    
} bitstream_t;

// Public functions

/* Create a bitstream using a buffer of a given size */
void create_bitstream(bitstream_t *bits, uint8_t *data, uint32_t length);

/* Free the data stored in the bitstream */
void delete_bitstream(bitstream_t *bits);

/* Read up to 64 bits. If there is a byte overflow, treat the result as little endian. */
uint64_t read_bits_little_endian(bitstream_t *bits, int N);

/* Read up to 64 bits. If there is a byte overflow, treat the result as big endian. */
uint64_t read_bits_big_endian(bitstream_t *bits, int N);

#endif
