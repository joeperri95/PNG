#include "png.h"

void readFileToBuffer(const char *filename, unsigned char **buf)
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

            printf("filesize %d\n", filesize);
            fseek(fp, 0, SEEK_SET);
          
            // allocate memory
            *buf = (unsigned char*) malloc(sizeof(unsigned char) * (filesize + 1));
            
            // read file to buf
            size_t numread = fread(*buf, sizeof(unsigned char), filesize, fp);
            *(*buf + numread + 1) = '\0';

        }
        else
        {
           LOG(ERROR, "Could not find end of file\n");
        }
    }
    else
    {
        printf("woah woah woah\n");
    }

    fclose(fp);
}

pngMetaData processHeader(unsigned char *buffer)
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
    unsigned char loop;
    unsigned char png_v[8] = {137, 80, 78, 71, 13, 10, 26, 10};
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


uint32_t getChunkLength(unsigned char *buffer)
{

	unsigned char loop;
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

void getChunkData(unsigned char *fileBuffer, uint32_t chunkLength, unsigned char *chunkBuffer)
{
        memcpy(chunkBuffer, fileBuffer, chunkLength * sizeof(unsigned char)); 
}


void getChunkType(unsigned char *buffer, char * str)
{
        memcpy(str, buffer, 4 * sizeof(char));
        str[5] = '\0';
}

uint32_t getCRC(unsigned char *buffer)
{

	uint32_t res = 0;

	res += ((uint32_t) *(buffer)) << 24;
	res += ((uint32_t) *(buffer + 1)) << 16;
	res += ((uint32_t) *(buffer + 2)) << 8;
	res += *(buffer + 3);

	return res;
}

bool validateCRC(uint32_t CRC)
{
    //  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
    // 0001 0000 0100 1100 0001 0001 1101 1011 0111 
    uint64_t CRC_polynomical =  0x104C11DB7; 
    


}
