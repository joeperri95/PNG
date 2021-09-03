#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

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

// This is where you set the log level
#define LOGGING_LEVEL INFO 
//#define LOGGING_LEVEL DEBUG_ALL 
//#define LOGGING_LEVEL 9 

#define LOGGING_ENABLED true 

// TODO turn this into a real library with timestamps, file logging and ANSI color codes

#define LOG_FILE "/dev/stdout"

void LOG(uint8_t level, const char* message, ...);


#endif
