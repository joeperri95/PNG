#ifndef __IMAGE_H__
#define __IMAGE_H__

// image.h
/*
    Take output of zlib stream and process into final image by applying
    defiltering algorithms as described in the PNG spec
*/

// Includes
#include "png.h"
#include "logging.h"

// Constants

#define FILTER_TYPE_NONE    0
#define FILTER_TYPE_SUB     1
#define FILTER_TYPE_UP      2
#define FILTER_TYPE_AVERAGE 3
#define FILTER_TYPE_PAETH   4

// Public Functions

/* Read the first byte of a scanline that indicates the filter type */
uint8_t read_filter_byte(uint8_t *buffer);

/* Read a scanline from the buffer */
void read_scanline(uint8_t *buffer, uint32_t length, uint8_t *scanline);

/* 
    NOT IMPLEMENTED
    Read a scanline using the adam7 interlacing format
*/
void read_scanline_adam7();

/* get bpp using the png metadata parameters */
uint8_t get_bytes_per_pixel(pngMetaData params);

/* Perform defiltering on the bytes of buffer and save the result to outputBuffer*/
void defilter(uint8_t *outputBuffer, uint8_t *buffer, uint32_t height, uint32_t scanwidth, uint8_t bpp);

/* copy the scanline to the output buffer */
void filter_none(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline);

/* compute output by summing the current byte in the scanline with the one in the previous pixel location. */
void filter_sub(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t bpp);

/* compute output by summing the current byte in the scanline with the one in the same pixel location of the previous scanline. */
void filter_up(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline);

/* compute output by summing the current byte in the scanline with the average of the pixel above and the pixel to the left. */
void filter_average(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline, uint8_t bpp);

/* compute output by summing the current byte in the scanline with the output of the paeth predictor algorithm */
void filter_paeth(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline, uint8_t bpp);

#endif
