#ifndef __GZIP_H__
#define __GZIP_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inflate.h"
#include "logging.h"

typedef struct _gzipMetaData
{
    uint32_t modified_time;
    uint32_t header_size; 
    uint8_t extraflags; 
    uint8_t compression_method;
    uint8_t flags;
    uint8_t os;  

} gzipMetaData;

bool gzip_validateFile(FILE *fp);

gzipMetaData gzip_processHeader(unsigned char* buffer);
uint32_t gzip_readFileToBuffer(const char*filename, unsigned char **buffer);

uint32_t gzip_getCRC32(unsigned char *buffer);
uint32_t gzip_getISIZE(unsigned char *buffer);

#endif
