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

    uint8_t *adler32Buffer;
    int64_t fileLength = get_file_length(filename); 
    read_file_to_buffer(filename, &adler32Buffer);
    
    // initialize global logger
    glog = create_logger("/dev/stdout", INFO);

    LOG(glog, INFO, "Computed ADLER32 0x%x\n", ADLER32(adler32Buffer, fileLength));
    free(adler32Buffer);

    destroy_logger(glog);

    return 0;
}
