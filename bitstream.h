#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"

typedef struct {
	uint32_t length;
	uint32_t byte_offset;
	uint8_t *buffer;
	uint8_t bit_offset;    
} bitstream_t;

void create_bitstream(bitstream_t *bits, uint8_t *data, uint32_t length);
void delete_bitstream(bitstream_t *bits);
uint8_t read_bits(bitstream_t *bits, int N);

#endif
