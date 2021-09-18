#ifndef __LOGGING_H__
#define __LOGGING_H__

// logging.h
/*
    A utility logging library
*/

// Includes
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ERROR 0
#define WARNING 1
#define INFO 2
#define DEBUG 3
#define DEBUG_1 4
#define DEBUG_2 5
#define DEBUG_3 6
// Log level 7 - 254 can be used freely
#define DEBUG_ALL 255 // max level

// Data Structures

typedef void (*func)(uint8_t level, const char *message, ...);

typedef struct _logger
{
    const char* filename;
    FILE *log_file;
    uint8_t level;
    bool enabled;    
} logger;

// The global logger object
logger *glog;

// Public Functions

logger *create_logger( const char* logfile, int level); 
void destroy_logger(logger *logger_handle);
void LOG(logger *logger_handle, uint8_t level, const char* message, ...);

// Private Functions

#endif
