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
    
    // initialize global logger
    glog = create_logger("/dev/stdout", INFO);

    FILE *fp = fopen(filename, "rb");
    if( !gzip_validate_file(fp) )
    {
        LOG(glog, ERROR, "Could not validate gzip file\n");
    }
    fclose(fp);
    
    uint32_t size = get_file_length(filename); 
    read_file_to_buffer(filename, &buffer);
    
    gzipMetaData gzip = gzip_process_header(buffer + offset); 
   
    offset += gzip.header_size;
    bitstream_t c;

    uint32_t crc = gzip_get_CRC32(buffer + size - 8);
    uint32_t isize = gzip_get_ISIZE(buffer + size - 4);
    
    create_bitstream(&c, buffer + offset, size - offset);
    output = malloc(isize * sizeof(unsigned char));

    uint32_t len = inflate(&c, output);
    
    printf("CRC %u %u\nISIZE %u\n", crc, CRC32(output, isize), isize);
    if(CRC32(output, isize) != crc)
    {
        LOG(glog, WARNING, "BAD CRC32\n");
    }
    else
    {
        LOG(glog, INFO, "Good CRC32\n");
    }

    if(c.bit_offset != 0)
    {
        c.bit_offset = 0;
        c.byte_offset++;
    }

    FILE *ofile = fopen("decompressed", "w");
    LOG(glog, DEBUG, "opened file\n");
    
    for(int i = 0; i < len; i ++)
    {
        printf("%c", output[i]);
        fprintf(ofile, "%c", output[i]);
    }

    fclose(ofile);

    destroy_logger(glog);
    return 0;
}
