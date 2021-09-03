#include "logging.h"

void timestamp()
{

}

void prefix(uint8_t level)
{
    switch(level)
    {
        case ERROR:
            printf(ANSI_COLOR_RED "ERROR: " ANSI_COLOR_RESET);
        break;

        case WARNING:
            printf(ANSI_COLOR_YELLOW "WARNING: " ANSI_COLOR_RESET);
        break;
    
        case INFO:
            printf(ANSI_COLOR_GREEN "INFO: " ANSI_COLOR_RESET);
        break;

        default:
        
            printf(ANSI_COLOR_MAGENTA "DEBUG %d: " ANSI_COLOR_RESET, level - 2);
        break;
    }
}

void LOG(uint8_t level, const char* message, ...)
{
    if(LOGGING_ENABLED == true)
    {
        if(level <= LOGGING_LEVEL)
        {
            
            prefix(level); 

            va_list variadic_arguments;
            va_start(variadic_arguments, message);
            vprintf(message, variadic_arguments); 
            va_end(variadic_arguments);
        }
    }
}
