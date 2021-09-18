#ifndef __PNG_H__
#define __PNG_H__

// png.h
/*

*/

// Includes
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "logging.h"
#include "inflate.h"
#include "checksum.h"
#include "bitstream.h"

// Constants
#define COLOR_TYPE_GRAYSCALE  	    0x00
#define COLOR_TYPE_RGB		  	    0x02
#define COLOR_TYPE_PALETTE	  	    0x03
#define COLOR_TYPE_GRAYSCALE_ALPHA  0x04
#define COLOR_TYPE_RGB_ALPHA 	    0x06

// Data Structures
/* 
	Contains metadata from header only
 	Additional metadata from nonessential chunks will be read elsewhere
*/
typedef struct _pngMetaData{
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;	
} pngMetaData;

/* An RGB triplet signifying a pallete entry */
typedef struct _pallete_pixel
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
} pallete_pixel;

/* A pallete as defined in a PLTE chunk */
typedef struct _pallete
{
	pallete_pixel buffer[256];
} pallete;

/*
	NOT USED 
	Something that could possibly be returned from the process chunk function
	Possibly add field for other chunk data

typedef struct _pngData
{
	uint8_t *outputData;
	uint32_t outputLength;
	pallete plte;
	pngMetaData pngHeader;
};
*/

// Public Functions

/* Given a file check if the PNG signature is correct*/
bool validate_PNG(FILE *fp);

/* Read the IHDR chunk and store metadata*/
pngMetaData process_PNG_header(uint8_t *buff);

/* 
	NOT YET COMPLETE
	Read the PNG chunks.
	Log ancillary chunk data for now.
	How do I get the pngMetadata and the pallete?
*/
void process_chunks(uint8_t *fileBuffer, uint8_t **outputBuffer, uint32_t *outputLength, pngMetaData *pngData, pallete *plte);

/* Read the 4 bytes for chunk length */
uint32_t get_chunk_length(uint8_t *buff);

/* 
	Read the 4 bytes corresponding to chunk type and store it in str. 
*/
void get_chunk_type(uint8_t *buff, char *str);

/* Store chunk data in the buffer*/
void get_chunk_data(uint8_t *fileBuffer, uint32_t chunkLength, uint8_t *chunkData);

/* Read the 4 bytes corresponding to the chunk CRC */
uint32_t get_CRC(uint8_t *buff);

/* Perform CRC on chunk and compare with read value */
bool validate_CRC(uint8_t* buff, uint32_t length, uint32_t CRC);

// Private Functions

/* tranform pallete data into RGB triplets */
void _decode_pallete(uint8_t *outputBuffer, uint8_t* inputBuffer, pngMetaData *params, pallete* plte);

/* Functions for handling specific PNG chunks*/

/* Critical Chunks */
pngMetaData _process_IHDR(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_IDAT(uint8_t *chunkBuffer, uint32_t chunkLength, uint8_t **outputBuffer, uint32_t *outputLength);
pallete _process_PLTE(uint8_t *chunkBuffer, uint32_t chunkLength);

/* Ancillary Chunks */
void _process_iTXt(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_zTXt(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_tEXt(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_sRGB(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_bKGD(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_pHYs(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_sBIT(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_sPLT(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_hIST(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_tIME(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_tRNS(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_gAMA(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_cHRM(uint8_t *chunkBuffer, uint32_t chunkLength);
void _process_iCCP(uint8_t *chunkBuffer, uint32_t chunkLength);

#endif
