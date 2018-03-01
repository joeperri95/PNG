#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
} pngMetaData;

typedef struct{
	pngMetaData metaData;
	
	unsigned char* dataBuffer;
	uint32_t dataLength;
	
} png;

//given a file check if the PNG signature is correct
bool validatePNG(FILE *fp);

PNGMetaData processHeader(char *chunkData);

//read the 4 bytes for chunk length
uint32_t getChunkLength(FILE *fp);

void getChunkType(FILE *fp,char *str);

//store chunk data in the buffer
void getChunkData(FILE *fp, uint32_t chunkLength,char *chunkData);

//detemine what to return
uint32_t getCRC(FILE *fp);

//determine how to validate
//look up CRC algorithm
bool validateCRC(uint32_t CRC);