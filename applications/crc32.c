#include "checksum.h"
#include "logging.h"
#include "fileutils.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    char filename[512];
    if(argc > 1)
    {
        strncpy(filename, argv[1], strlen(argv[1]));         
        printf("%s\n", filename);
    }
    else
    {
        return -1;
    }

    // initialize global logger
    glog = create_logger("/dev/stdout", INFO);

    uint8_t *crc32Buffer;
    int64_t fileLength = get_file_length(filename); 
    read_file_to_buffer(filename, &crc32Buffer);
    
    LOG(glog, INFO, "Computed CRC32 0x%x\n", CRC32(crc32Buffer, fileLength));
    free(crc32Buffer);

    destroy_logger(glog);
    return 0;
}
