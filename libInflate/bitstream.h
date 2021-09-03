#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"

// Bitstream object containing a data buffer and the current byte and bit offset
typedef struct {
	uint32_t length;
	uint32_t byte_offset;
	uint8_t *buffer;
	uint8_t bit_offset;    
} bitstream_t;

// Create a bitstream using a buffer of a given size
void create_bitstream(bitstream_t *bits, uint8_t *data, uint32_t length);

// Free associated data
void delete_bitstream(bitstream_t *bits);

// Read up to 64 bits
uint64_t read_bits_little_endian(bitstream_t *bits, int N);
uint64_t read_bits_big_endian(bitstream_t *bits, int N);

#endif
