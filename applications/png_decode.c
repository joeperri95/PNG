#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include "inflate.h"
#include "logging.h"
#include "gzip.h"
#include "checksum.h"
#include "fileutils.h"
#include "image.h"

int main(int argc, char **argv)
{	
	
	char filename[512];
	if (argc > 1)
	{
		strncpy(filename, argv[1], strlen(argv[1]));
	}
	else
	{
		strncpy(filename, "res/sample3.png", 16);
	}

	// initialize global logger
    glog = create_logger("/dev/stdout", DEBUG_3);

	unsigned char *pngBuffer, *output; 
	uint32_t offset = 8;
 	FILE *fp = fopen(filename, "rb");

	if (!validate_PNG(fp))
	{
            LOG(glog, ERROR, "Could not validate png file\n");
            return -1;
	}

	fclose(fp);

	read_file_to_buffer(filename, &pngBuffer);

	pngMetaData pngData; 
	char chunkType[5] = "1234";
	
	bool png_quit = false; 
	uint32_t chunkLength = 0;

	uint8_t *dataStream = (uint8_t *)malloc(sizeof(uint8_t *));
	uint8_t *chunkBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
	uint8_t *crcBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
	uint32_t dataLength = 0;

	
	process_chunks(pngBuffer + offset, &dataStream, &dataLength, &pngData);
	
	//zlib section
       
	int bpp = pngData.bit_depth / 8;
	
	switch(pngData.color_type)
	{
		case COLOR_TYPE_GRAYSCALE:
			// grayscale and no alpha channel. Do not need to modify bpp
		break;

		case COLOR_TYPE_RGB:
			// rgb triplet
			bpp *= 3;
		break;
		case COLOR_TYPE_PALETTE:
			// using a pallete. This is currently not supported
                        LOG(glog, ERROR, "Pallete chunks not supported\n");
                        exit(1);
		break;
		
		case COLOR_TYPE_GRAYSCALE_ALPHA:
			// grayscale with an alpha channel
			bpp *= 2;
		break;
		case COLOR_TYPE_RGB_ALPHA:
			// rgb with and alpha channel
			bpp *= 4;
		break;

		default:
		break;
	}

	int scanwidth = pngData.width * bpp + 1;

    uint32_t uncompressed_size = (scanwidth) * pngData.height;
	uint8_t *outputStream = malloc(uncompressed_size * sizeof(uint8_t));

	bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	zlibMetaData z_data = process_zlib_header(&b);
	
	uint32_t len = inflate(&b, outputStream);

        if(len != uncompressed_size)
        {
                LOG(glog, WARNING, "inflate size wrong\n");
        }

	uint32_t adler = read_ADLER32(&b);
	uint32_t adler_computed = ADLER32(outputStream, len);

	if(b.byte_offset != b.length)
	{
			LOG(glog, WARNING, "Extra bytes in bitstream\n");
	}

	if(b.bit_offset != 0)
	{
			LOG(glog, WARNING, "Bitstream has offset\n");
	}

	if(adler != adler_computed)
	{
		LOG(glog, WARNING, "BAD ADLER32 0x%x != 0x%x\n", adler, adler_computed);
	}
	else
	{
		LOG(glog, INFO , "GOOD ADLER32 0x%x\n",adler);
	}

    
	delete_bitstream(&b);

	uint8_t *defiltered = malloc(pngData.height * pngData.width * bpp);
	defilter( defiltered , outputStream, pngData);
	
	FILE *decomp = fopen("outputlog.hex", "w");
	for(int i = 0; i < scanwidth * pngData.height; i++)
	{
		fputc(outputStream[i], decomp);
	}
        
	fclose(decomp);

	FILE *log = fopen("outputlog.ppm", "w");
	fprintf(log, "P6\n%d %d\n%d\n", pngData.width, pngData.height, (1 << pngData.bit_depth) - 1);

	for(int i = 0; i < (scanwidth - 1) * pngData.height; i++)
	{
            if(pngData.color_type == 6)
            {
                if(bpp == 8){

                    if((( (i + 1) % 7)!= 0) && (( (i + 1)  % 8) != 0)){
                        fputc(defiltered[i], log);
                    }
                }
                else
                {

                    if(((i + 1) % 4 != 0)){
                        fputc(defiltered[i], log);
                    }
                }
            }
            else
            {
                fputc(defiltered[i], log);
            }
	}

	fclose(log);
	destroy_logger(glog);
    
	return 0;
	
	free(dataStream);
	free(pngBuffer);
	delete_bitstream(&b);
	getchar();

	return 1;

}
