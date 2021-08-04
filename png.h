#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"

#define BFINAL 0x01
#define BTYPE 0x06
#define NO_COMPRESSION 0
#define FIXED_HUFFMAN 1
#define DYNAMIC_HUFFMAN 2
#define RESERVED_ERROR 3

// Contains metadata from header only
// Additional metadata from nonessential chunks will be read elsewhere
typedef struct _pngMetaData{
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;	
} pngMetaData;

void readFileToBuffer(const char* filename, unsigned char **buf);

// Given a file check if the PNG signature is correct
bool validatePNG(FILE *fp);

// Read the IHDR chunk and store metadata
pngMetaData processHeader(unsigned char *buff);

// Read the 4 bytes for chunk length
uint32_t getChunkLength(unsigned char *buff);

// Read the 4 bytes corresponding to chunk type
void getChunkType(unsigned char *buff,char *str);

// Store chunk data in the buffer
void getChunkData(unsigned char *buff, uint32_t chunkLength, unsigned char *chunkData);

// Read the 4 bytes corresponding to the chunk CRC 
uint32_t getCRC(unsigned char *buff);

// UNIMPLEMENTED. Perform CRC on chunk and compare with read value
bool validateCRC(uint32_t CRC);
