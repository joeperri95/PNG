#ifndef __INFLATE_H__
#define __INFLATE_H__

// inflate.h
/*
	This module performs the inflate operation. There are also utilities for dealing with a zlib stream 
	including reading the zlib header and the ADLER32 checksum
*/

// Includes
#include "bitstream.h"
#include "huffman.h"

// Constants

#define COMPRESSION_METHOD_NONE    0x00
#define COMPRESSION_METHOD_FIXED   0x01
#define COMPRESSION_METHOD_DYNAMIC 0x02

// Data Structures

/* Metadata included in the header of a zlib stream */
typedef struct _zlibMetaData
{
	uint8_t compression_method;
	uint8_t compression_info;
	uint8_t fcheck;
	uint8_t fdict;
	uint8_t flevel;
} zlibMetaData;


// Public Functions 

/* Retrieve metadata from start of zlib stream  */
zlibMetaData process_zlib_header(bitstream_t *input);

/* Read and process deflate streams until complete. Write decompressed data to outputStream. Return number of bytes written to ouput stream */
uint32_t inflate(bitstream_t *input, unsigned char *outputStream);

/* Copy an uncompressed stream */
void uncompressed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);

/* Decompress a stream that was compressed using dynamic codes */
void compressed_dynamic(bitstream_t *input, unsigned char* outputStream, uint32_t *outputLength);

/* Decompress a stream that was compressed using fixed codes */
void compressed_fixed(bitstream_t *input, unsigned char *outputStream, uint32_t *outputLength);

/* Read the final 4 bytes in the bitstream */
uint32_t read_ADLER32(bitstream_t *input);

#endif
