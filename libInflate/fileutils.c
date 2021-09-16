#include "fileutils.h"

int64_t get_file_length(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    int64_t filesize = -1;

    if(fp != NULL)
    {
        if(fseek(fp, 0, SEEK_END) == 0)
        {
            filesize = ftell(fp);
        }
        else
        {
            LOG(glog, ERROR, "Could not find end of file\n");
        }
    }
    else
    {
        LOG(glog, ERROR, "Could not open file\n");
    }
    
    fclose(fp);
    return filesize;
}

void read_file_to_buffer(const char *filename, uint8_t **buf)
{

    int64_t filelength =  get_file_length(filename);
    LOG(glog, DEBUG, "file length %d\n", filelength);
 
    FILE *fp = fopen(filename, "rb");

    if(fp != NULL)
    {
        *buf = (uint8_t *) malloc(sizeof(uint8_t) * (filelength + 1));
        if(*buf == NULL)
        {
            LOG(glog, ERROR, "Could not allocate memory\n");
        }
        size_t numread = fread(*buf, sizeof(uint8_t), filelength, fp); 
        *(*buf + numread + 1) = '\0';
    }
    else
    {
        LOG(glog, ERROR, "Could not open file\n");
    }
}
