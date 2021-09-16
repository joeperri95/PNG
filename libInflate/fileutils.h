#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

// fileutils.h
/*
     Common file utility functions needed between modules
*/

// Includes
#include "logging.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Public Functions

/* return number of bytes in file */
int64_t get_file_length(const char* filename);

/* read file to auto allocated buffer */
void read_file_to_buffer(const char* filename, uint8_t **buffer);

#endif
