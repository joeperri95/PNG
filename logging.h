#ifndef __LOGGING_H__
#define __LOGGING_H__

// comment this line out to turn off logging completely
#define LOGGING 1
#include <stdio.h>

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

// TODO turn this into a real library with timestamps, file logging and ANSI color codes

#ifdef LOGGING

    #define LOG(level, ...) if((level) <= LOGGING_LEVEL ) { printf( __VA_ARGS__); }

#else
    #define LOG 
#endif

#endif
