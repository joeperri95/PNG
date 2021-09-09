#include "gzip.h"

bool gzip_validateFile(FILE *fp)
{
    unsigned char ID1 = fgetc(fp);        
    unsigned char ID2 = fgetc(fp);       

    if(ID1 == 0x1F && ID2 == 0x8B)
    {    
        return true;
    }
    else
    {
        return false;
    }

}

gzipMetaData gzip_processHeader(unsigned char* buffer)
{
    gzipMetaData gzipData;
    uint32_t offset = 0;
    
    gzipData.compression_method = *(buffer + offset++);    
    gzipData.flags = *(buffer + offset++);
    
    uint32_t mtime = *(buffer + offset++); 
    mtime = (mtime) << 0x08 + *(buffer + offset++);
    mtime = (mtime) << 0x08 + *(buffer + offset++);
    mtime = (mtime) << 0x08 + *(buffer + offset++);
    
    gzipData.modified_time = mtime;

    gzipData.extraflags = *(buffer + offset++);
    gzipData.os = *(buffer + offset++);

    if(gzipData.flags & 0x01)
    {
        uint16_t extralen = *(buffer + offset++); 
        extralen = (extralen) << 0x08 + *(buffer + offset++);

        for(int i = 0; i < extralen; i++)
        {
            offset++;
        }
    }

    if(gzipData.flags & 0x08)
    {
        
        printf("%s\n", buffer + offset);

        unsigned char loop = *(buffer + offset++);
        while(loop != '\0')
        {
            loop = *(buffer + offset++);
        }
    }

    if(gzipData.flags & 0x10)
    {
        unsigned char loop = *(buffer + offset++);
        while(loop != '\0')
        {
            loop = *(buffer +offset++);
        }

    }

    if(gzipData.flags & 0x02)
    {
        offset += 2;
    }

    gzipData.header_size = offset;

    LOG(INFO, "Compression type %d\n", gzipData.compression_method);
    LOG(INFO, "Flags %d\n", gzipData.flags);
    LOG(INFO, "Modified %d\n", gzipData.modified_time);
    LOG(INFO, "Extra flags %d\n", gzipData.extraflags);
    LOG(INFO, "OS %d\n", gzipData.os);
    LOG(INFO, "Header size %d\n", gzipData.header_size);

    return gzipData;
}

uint32_t gzip_readFileToBuffer(const char*filename, unsigned char **buffer)
{

    FILE *fp = fopen(filename, "rb");
    long filesize = 0;
    size_t numread;
    if(fp != NULL)
    {
        if(fseek(fp, 0, SEEK_END) == 0)
        {
            //get file size
            filesize = ftell(fp);
            if(filesize == -1)
                LOG(ERROR, "Could not determine filesize\n");

            fseek(fp, 0, SEEK_SET);
          
            // allocate memory
            *buffer = (unsigned char*) malloc(sizeof(unsigned char) * (filesize + 1));
            
            // read file to buf
             numread = fread(*buffer, sizeof(unsigned char), filesize, fp);
            *(*buffer + numread + 1) = '\0';

        }
        else
        {
           LOG(ERROR, "Could not find end of file\n");
        }
    }
    else
    {
        LOG(ERROR, "Could not open file\n");
    }

    fclose(fp);
    return numread;
}

uint32_t gzip_getCRC32(unsigned char *buffer)
{
    uint8_t offset = 0;
    
    uint32_t ret = *(buffer + offset++);

    ret += (*(buffer + offset++) << 8);
    ret += (*(buffer + offset++) << 16);
    ret += (*(buffer + offset++) << 24);
   
   return ret;

}

uint32_t gzip_getISIZE(unsigned char *buffer){

    uint8_t offset = 0;
    uint32_t ret = *(buffer + offset++);

    ret += (*(buffer + offset++) << 8);
    ret += (*(buffer + offset++) << 16);
    ret += (*(buffer + offset++) << 24);
    
    return ret;

}
