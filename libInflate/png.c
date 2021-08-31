#include "png.h"

void readFileToBuffer(const char *filename, uint8_t **buf)
{
  
    FILE *fp = fopen(filename, "rb");
    long filesize = 0;

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
            *buf = (uint8_t*) malloc(sizeof(uint8_t) * (filesize + 1));
            
            // read file to buf
            size_t numread = fread(*buf, sizeof(uint8_t), filesize, fp);
            *(*buf + numread + 1) = '\0';

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
}

pngMetaData processHeader(uint8_t *buffer)
{
    pngMetaData pngData;

    int offset = 0;
    pngData.width = *(buffer + offset++) << 24;
    pngData.width += *(buffer + offset++) << 16;
    pngData.width += *(buffer + offset++) << 8;
    pngData.width += *(buffer + offset++);

    pngData.height = *(buffer + offset++) << 24;
    pngData.height += *(buffer + offset++) << 16;
    pngData.height += *(buffer + offset++) << 8;
    pngData.height += *(buffer + offset++);

    pngData.bit_depth = *(buffer + offset++);
    pngData.color_type = *(buffer + offset++);
    pngData.compression_method = *(buffer + offset++);
    pngData.filter_method = *(buffer + offset++);
    pngData.interlace_method = *(buffer + offset++);

    LOG(INFO, "Width %" PRIu32 "\n", pngData.width);
    LOG(INFO, "Height %" PRIu32 "\n", pngData.height);
    LOG(INFO, "Bit Depth %d\n", pngData.bit_depth);
    LOG(INFO, "Color Type %d\n", pngData.color_type);
    LOG(INFO, "Compression Method %d\n", pngData.compression_method);
    LOG(INFO, "Filter Method %d\n", pngData.filter_method);
    LOG(INFO, "Interlace Method %d\n", pngData.interlace_method);

    return pngData;
}

bool validatePNG(FILE *fp)
{
    uint8_t loop;
    uint8_t png_v[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    bool res = true;

    for (int i = 0; i < 8; i++)
    {
        loop = fgetc(fp);

        if (loop != png_v[i])
        {
            res = false;
        }
    }

    return res;
}


uint32_t getChunkLength(uint8_t *buffer)
{

	uint8_t loop;
	uint32_t res = 0;

	loop = *(buffer++);
	res += (uint32_t)loop << 24;

	loop = *(buffer++); 
	res += (uint32_t)loop << 16;
        
        loop = *(buffer++); 
	res += (uint32_t)loop << 8;
        
        loop = *(buffer++); 
	res += (uint32_t)loop;

	return res;
}

void getChunkData(uint8_t *fileBuffer, uint32_t chunkLength, uint8_t *chunkBuffer)
{
        memcpy(chunkBuffer, fileBuffer, chunkLength * sizeof(uint8_t)); 
}


void getChunkType(uint8_t *buffer, char * str)
{
        memcpy(str, buffer, 4 * sizeof(char));
        str[5] = '\0';
}

uint32_t getCRC(uint8_t *buffer)
{

	uint32_t res = 0;

	res += ((uint32_t) *(buffer)) << 24;
	res += ((uint32_t) *(buffer + 1)) << 16;
	res += ((uint32_t) *(buffer + 2)) << 8;
	res += *(buffer + 3);

	return res;
}

bool validateCRC(uint8_t* buffer, uint32_t length, uint32_t CRC)
{
    //  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
    // 0001 0000 0100 1100 0001 0001 1101 1011 0111 
    
    uint32_t ret = CRC32(buffer, length);
    printf("0x%x\n", ret);
    return ( ret == CRC);

}

uint32_t CRC32(uint8_t *buffer, uint32_t len)
{
    // perform a crc32 using a table as outlined in rfc2083 

    uint64_t c;
    uint64_t crc_register;       
    uint64_t crc_table[256];
        
    // make crc table
    for (int i = 0; i < 256; i++) {
        c = i;
        
        for (int j = 0; j < 8; j++) {
            c = (c & 1) ? 0xedb88320L ^ (c >> 1) : c >> 1;
        }

        crc_table[i] = c;
    }

    crc_register = 0xffffffffL;
    
    for (int i = 0; i < len; i++) {
        crc_register = crc_table[(crc_register ^ buffer[i]) & 0xff] ^ (crc_register >> 8);
    }

    return (uint32_t) (crc_register ^ 0xffffffffL);
}
