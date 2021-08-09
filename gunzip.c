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
    gzip_validateFile(fp);
    fclose(fp);
    
    uint32_t size = gzip_readFileToBuffer(filename, &buffer);
    gzipMetaData gzip = gzip_processHeader(buffer + offset); 
   
    offset += gzip.header_size;
    bitstream_t c;

    create_bitstream(&c, buffer + offset, size  - offset);

    output = malloc(10000 * sizeof(unsigned char));
    z_inflate(&c, output);
    
    for(int i = 0; i < 10000; i ++)
    {
        printf("%c", output[i]);
    }

}
