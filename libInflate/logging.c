#include "logging.h"

// The global logger
// This may not be the best way to do it but it means I can call LOG from anywhere
extern logger *glog = NULL;

void timestamp(logger *logger_handle)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    fprintf(logger_handle->log_file, ANSI_COLOR_CYAN "[%d-%d-%d %d:%d:%d] " ANSI_COLOR_RESET, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, 
            timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
}

void prefix(logger *logger_handle, uint8_t level)
{
    switch(level)
    {
        case ERROR:
            fprintf(logger_handle->log_file ,ANSI_COLOR_RED "ERROR: " ANSI_COLOR_RESET);
        break;

        case WARNING:
            fprintf(logger_handle->log_file, ANSI_COLOR_YELLOW "WARNING: " ANSI_COLOR_RESET);
        break;
    
        case INFO:
            fprintf(logger_handle->log_file, ANSI_COLOR_GREEN "INFO: " ANSI_COLOR_RESET);
        break;

        default:
            fprintf(logger_handle->log_file, ANSI_COLOR_MAGENTA "DEBUG %d: " ANSI_COLOR_RESET, level - 2);
        break;
    }
}

void LOG(logger *logger_handle, uint8_t level, const char* message, ...)
{
    if(logger_handle != NULL){
        if(logger_handle->enabled == true)
        {
            if(level <= logger_handle->level)
            {

                timestamp(logger_handle);
                prefix(logger_handle, level); 

                va_list variadic_arguments;
                va_start(variadic_arguments, message);
                vfprintf(logger_handle->log_file, message, variadic_arguments); 
                va_end(variadic_arguments);
            }
        }
    }
}

logger *create_logger(const char* logfile, int level)
{
    logger *logger_handle = malloc(sizeof(logger));

    logger_handle->log_file = fopen(logfile, "ab");
    logger_handle->filename = logfile;
    logger_handle->level = level;    
    logger_handle->enabled = true;

    return logger_handle;
}

void destroy_logger(logger *logger_handle)
{    
    if(logger_handle != NULL)
    {
        fclose(logger_handle->log_file);    
        free(logger_handle);
        logger_handle = NULL;
    }
}