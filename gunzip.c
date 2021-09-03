#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include "inflate.h"
#include "logging.h"
#include "gzip.h"

// This is the entry point for the gunzip implementation

int main(int argc, char **argv)
{

    char filename[512];
    if (argc > 1)
    {
            strncpy(filename, argv[1], strlen(argv[1]));
    }
    else
    {
        return 1;
    }

    unsigned char *buffer, *output; 
    uint32_t offset = 2;
    
    FILE *fp = fopen(filename, "rb");
    if( !gzip_validateFile(fp) )
    {
        LOG(ERROR, "Could not validate gzip file\n");
    }
    fclose(fp);
    
    uint32_t size = gzip_readFileToBuffer(filename, &buffer);
    gzipMetaData gzip = gzip_processHeader(buffer + offset); 
   
    offset += gzip.header_size;
    bitstream_t c;

    uint32_t crc = gzip_getCRC32(buffer + size - 12);
    uint32_t isize = gzip_getISIZE(buffer + size - 8);
   
    printf("CRC %d\nISIZE %d\n", crc, isize);
    create_bitstream(&c, buffer + offset, size  - offset - 8);

    output = malloc(isize * sizeof(unsigned char));

    uint32_t len = z_inflate(&c, output);
    
    uint32_t adler = z_readADLER32(&c);
    uint32_t adlerComputed = z_ADLER32(output, len);
    
    if(adler != adlerComputed)
    {
        LOG(WARNING, "BAD ADLER32 0x%x != 0x%x\n", adler, adlerComputed);
    }
    else
    {
        LOG(INFO, "GOOD ADLER32\n");
    }

    for(int i = 0; i < len; i ++)
    {
        printf("%c", output[i]);
    }

}
