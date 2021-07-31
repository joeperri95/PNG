#ifndef __INFLATE_H__
#define __INFLATE_H__

#include "bitstream.h"
#include "huffman.h"

typedef struct _zlibMetaData
{
	uint8_t compression_method;
	uint8_t compression_info;
	uint8_t fcheck;
	uint8_t fdict;
	uint8_t flevel;
} zlibMetaData;


zlibMetaData z_processHeader(bitstream_t *input);

void z_inflate(bitstream_t *input, unsigned char *outputStream);
void z_uncompressed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);
void z_compressed_dynamic(bitstream_t *input, unsigned char* outputStream, uint32_t *outputLength);
void z_compressed_fixed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);

void z_ADLER32();

#endif
