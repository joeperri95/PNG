#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define BFINAL 0x01
#define BTYPE 0x06
#define NO_COMPRESSION 0
#define FIXED_HUFFMAN 1
#define DYNAMIC_HUFFMAN 2
#define RESERVED_ERROR 3


//contains metadata from header only
//Additional metadata from nonessential chunks will be read elsewhere
typedef struct{
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
	bool error;
} pngMetaData;

unsigned char* getFilePointer(const char* filename);

//given a file check if the PNG signature is correct
bool validatePNG(unsigned char **buff);

pngMetaData processHeader(unsigned char **buff);

//read the 4 bytes for chunk length
uint32_t getChunkLength(unsigned char **buff);

void getChunkType(unsigned char **buff,char *str);

//store chunk data in the buffer
void getChunkData(unsigned char **buff, uint32_t chunkLength,char *chunkData);

//detemine what to return
uint32_t getCRC(unsigned char *buff);

//determine how to validate
//look up CRC algorithm
bool validateCRC(uint32_t CRC);