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
    printf("%x %x\n", fgetc(fp), fgetc(fp));    
    if( gzip_validateFile(fp) )
    {
        printf("ohKAY\n");
    }
    fclose(fp);
    
    uint32_t size = gzip_readFileToBuffer(filename, &buffer);
    gzipMetaData gzip = gzip_processHeader(buffer + offset); 
   
    offset += gzip.header_size;
    bitstream_t c;

    create_bitstream(&c, buffer + offset, size  - offset);

    output = malloc(10000 * sizeof(unsigned char));

    uint32_t len = z_inflate(&c, output);
    
    uint32_t adler = z_readADLER32(&c);
    uint32_t adlerComputed = z_ADLER32(output, len);
    
    if(adler != adlerComputed)
    {
        printf("BAD ADLER32 0x%x != 0x%x\n", adler, adlerComputed);
    }

    for(int i = 0; i < len; i ++)
    {
        printf("%c", output[i]);
    }

}
