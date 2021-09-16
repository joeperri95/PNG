#ifndef __GZIP_H__
#define __GZIP_H__

// gzip.h
/*
    A module for handling the metatdata surrounding gzip files.
*/

// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inflate.h"
#include "logging.h"
#include "fileutils.h"

// Data structures

/* The metadata contained in a gzip header */
typedef struct _gzipMetaData
{
    uint32_t modified_time;
    uint32_t header_size; 
    uint8_t extraflags; 
    uint8_t compression_method;
    uint8_t flags;
    uint8_t os;  

} gzipMetaData;

// Public Functions

/* Ensure that the gzip magic numbers are present at the start of the file. */
bool gzip_validate_file(FILE *fp);

/* Parse the information associated with the gzip file */
gzipMetaData gzip_process_header(unsigned char* buffer);

/* Return the CRC32 attached to the end of the file */
uint32_t gzip_get_CRC32(unsigned char *buffer);

/* Return the uncompressed size attached to the end of the file */
uint32_t gzip_get_ISIZE(unsigned char *buffer);

#endif
