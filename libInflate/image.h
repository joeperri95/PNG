#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "png.h"
#include "logging.h"

// Take output of zlib stream and process into final image

// Module constants

#define FILTER_TYPE_NONE    0
#define FILTER_TYPE_SUB     1
#define FILTER_TYPE_UP      2
#define FILTER_TYPE_AVERAGE 3
#define FILTER_TYPE_PAETH   4

// Module methods

uint8_t read_filter_byte(uint8_t *buffer);
void read_scanline(uint8_t *buffer, uint32_t length, uint8_t *scanline);
void read_scanline_adam7();

void defilter(uint8_t *outputBuffer, uint8_t *buffer, pngMetaData params);

void filter_none(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline);
void filter_sub(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t bpp);
void filter_up(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline);
void filter_average(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline, uint8_t bpp);
void filter_paeth(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline, uint8_t bpp);

#endif
