#include "gzip.h"

bool gzip_validate_file(FILE *fp)
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

gzipMetaData gzip_process_header(unsigned char* buffer)
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

    LOG(glog, INFO, "Compression type %d\n", gzipData.compression_method);
    LOG(glog, INFO, "Flags %d\n", gzipData.flags);
    LOG(glog, INFO, "Modified %d\n", gzipData.modified_time);
    LOG(glog, INFO, "Extra flags %d\n", gzipData.extraflags);
    LOG(glog, INFO, "OS %d\n", gzipData.os);
    LOG(glog, INFO, "Header size %d\n", gzipData.header_size);

    return gzipData;
}

uint32_t gzip_get_CRC32(unsigned char *buffer)
{
    uint8_t offset = 0;
    
    uint32_t ret = *(buffer + offset++);

    ret += (*(buffer + offset++) << 8);
    ret += (*(buffer + offset++) << 16);
    ret += (*(buffer + offset++) << 24);
   
   return ret;

}

uint32_t gzip_get_ISIZE(unsigned char *buffer){

    uint8_t offset = 0;
    uint32_t ret = *(buffer + offset++);

    ret += (*(buffer + offset++) << 8);
    ret += (*(buffer + offset++) << 16);
    ret += (*(buffer + offset++) << 24);
    
    return ret;

}
