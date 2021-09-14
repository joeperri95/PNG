#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include "inflate.h"
#include "logging.h"
#include "gzip.h"
#include "checksum.h"

// This is the entry point for the gunzip implementation

int main(int argc, char **argv)
{

    char filename[1024];
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

    uint32_t crc = gzip_getCRC32(buffer + size - 8);
    uint32_t isize = gzip_getISIZE(buffer + size - 4);
    
    create_bitstream(&c, buffer + offset, size - offset);
    output = malloc(isize * sizeof(unsigned char));

    uint32_t len = z_inflate(&c, output);
    
    printf("CRC %u %u\nISIZE %u\n", crc, CRC32(output, isize), isize);
    if(CRC32(output, isize) != crc)
    {
        LOG(WARNING, "BAD CRC32\n");
    }
    else
    {
        LOG(INFO, "Good CRC32\n");
    }

    if(c.bit_offset != 0)
    {
        c.bit_offset = 0;
        c.byte_offset++;
    }

    FILE *ofile = fopen("decompressed", "w");
    LOG(DEBUG, "opened file\n");
    
    for(int i = 0; i < len; i ++)
    {
        printf("%c", output[i]);
        fprintf(ofile, "%c", output[i]);
    }

    fclose(ofile);
    return 0;
}
