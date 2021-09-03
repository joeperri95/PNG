#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include <stdint.h>
#include "logging.h"

// A module for the checksums needed by both PNG and GZIP applications

uint32_t CRC32(uint8_t *buffer, uint32_t len);
uint32_t CRC16(uint8_t *buffer, uint32_t len);
uint32_t ADLER32(uint8_t* buffer, uint32_t length);

#endif
