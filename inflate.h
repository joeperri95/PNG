#ifndef __INFLATE_H__
#define __INFLATE_H__

#include "bitstream.h"
#include "huffman.h"

// Metadata included in the header of a zlib stream
typedef struct _zlibMetaData
{
	uint8_t compression_method;
	uint8_t compression_info;
	uint8_t fcheck;
	uint8_t fdict;
	uint8_t flevel;
} zlibMetaData;

// Retrieve metadata from start of zlib stream 
zlibMetaData z_processHeader(bitstream_t *input);

// Read and process zlib streams until complete. Write decompressed data to outputStream
void z_inflate(bitstream_t *input, unsigned char *outputStream);

// Copy an uncompressed stream
void z_uncompressed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);

// Decompress a stream that was compressed using dynamic codes
void z_compressed_dynamic(bitstream_t *input, unsigned char* outputStream, uint32_t *outputLength);

// UNIMPLEMENTED. Decompress a stream that was compressed using fixed codes
void z_compressed_fixed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);

// Read the final 4 bytes in the bitstream
uint32_t z_readADLER32(bitstream_t *input);

// UNIMPLEMENTED. Compute ADLER32 checksum
void z_ADLER32();

#endif
