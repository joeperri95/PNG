#include "logging.h"

void timestamp()
{

}

void level(uint8_t level)
{
    switch(level)
    {
        case ERROR:
            printf(ANSI_COLOR_RED "ERROR: " ANSI_COLOR_RESET);
        break;

        case WARNING:
            printf(ANSI_COLOR_YELLOW "WARNING: " ANSI_COLOR_RESET);
        break;

    }
}

void LOG(uint8_t level, const char* message, ...)
{
    if(LOGGING_ENABLED == true)
    {
        if(level <= LOGGING_LEVEL)
        {

            va_list variadic_arguments;
            va_start(variadic_arguments, message);
            vprintf(message, variadic_arguments); 
            va_end(variadic_arguments);
        }
    }
}
