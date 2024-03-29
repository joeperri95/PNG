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

	// initialize global logger
        glog = create_logger("/dev/stdout", DEBUG_3 + 1);

	char out_filename[1024];
	char filename[1024];
    
	if (argc > 1)
	{
		printf("%s\n", argv[1]);
		strcpy(filename, argv[1]);
		strcpy(out_filename, argv[1]);
        
        // replace extension with .ppm
        for(int i = strlen(out_filename); i >= 0; i--)
        {
            if(out_filename[i] == '.')
            {
                memmove(out_filename + i, ".ppm", 5);                
                break;
            }
        }

	}
	else
	{
		LOG(glog, INFO, "Please supply a filename\n");
        return -1;
	}
	

	unsigned char *pngBuffer; 
	
	// data starts 8 bytes in after the PNG signature
	uint32_t offset = 8;
 	
	// check PNG signature
	FILE *fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		LOG(glog, ERROR, "Could not open file %s\n", filename);
		return -1;
	}

	if (!validate_PNG(fp))
	{			
            LOG(glog, ERROR, "Could not validate png file\n");		
	        return -1;
	}
	
	fclose(fp);

	read_file_to_buffer(filename, &pngBuffer);

	pngMetaData pngData; 		
	uint8_t *dataStream = (uint8_t *)malloc(sizeof(uint8_t *));
	uint32_t dataLength = 0;

    pallete plte;	
	process_chunks(pngBuffer + offset, &dataStream, &dataLength, &pngData, &plte);

	// bytes per pixel
	int bpp = get_bytes_per_pixel(pngData); 
    
    int scanwidth;
    uint8_t *outputStream;
    uint8_t *outputPLTEStream;
    uint32_t uncompressed_size; 
    if(pngData.interlace_method == 1)
    {            
        
        scanwidth = pngData.width * bpp + 1;
        
        // size of each scanline
        int scan1 = (pngData.height >> 0x03) * (1 + ((pngData.width * bpp) >> 0x03));
        int scan2 = (pngData.height >> 0x03) * (1 + ((pngData.width * bpp) >> 0x03));
        int scan3 = (pngData.height >> 0x03) * (1 + ((pngData.width * bpp) >> 0x02));
        int scan4 = (pngData.height >> 0x02) * (1 + ((pngData.width * bpp) >> 0x02));
        int scan5 = (pngData.height >> 0x02) * (1 + ((pngData.width * bpp) >> 0x01));
        int scan6 = (pngData.height >> 0x01) * (1 + ((pngData.width * bpp) >> 0x01));
        int scan7 = (pngData.height >> 0x01) * ((pngData.width * bpp) + 1);
        
        uncompressed_size = scan1 + scan2 + scan3 + scan4 + scan5 + scan6 + scan7;
        
        LOG(glog, INFO, "Scan size 1 %d\n", scan1); 
        LOG(glog, INFO, "Scan size 2 %d\n", scan2); 
        LOG(glog, INFO, "Scan size 3 %d\n", scan3); 
        LOG(glog, INFO, "Scan size 4 %d\n", scan4); 
        LOG(glog, INFO, "Scan size 5 %d\n", scan5); 
        LOG(glog, INFO, "Scan size 6 %d\n", scan6); 
        LOG(glog, INFO, "Scan size 7 %d\n", scan7); 

        LOG(glog, INFO, "Decompressed size %d\n", uncompressed_size); 
        outputStream = malloc(uncompressed_size * sizeof(uint8_t));
        outputPLTEStream = malloc(uncompressed_size * sizeof(uint8_t));
    }
    else
    {

        // The size of one scanline
        scanwidth = pngData.width * bpp + 1;
        uncompressed_size = (scanwidth) * pngData.height;
        outputStream = malloc(uncompressed_size * sizeof(uint8_t));
        outputPLTEStream = malloc(uncompressed_size * sizeof(uint8_t));
    }

    bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	zlibMetaData z_data = process_zlib_header(&b);
	
	// The inflate operation
	uint32_t len = inflate(&b, outputStream);
	if(len != uncompressed_size)
	{
			LOG(glog, WARNING, "Inflate size computed %d does not match expected size %d\n", len, uncompressed_size);
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

    if(pngData.color_type == COLOR_TYPE_PALETTE){
        decode_pallete(outputPLTEStream, outputStream, pngData, plte);
        defilter(defiltered, outputPLTEStream, pngData.height,  (bpp * pngData.width + 1), bpp);
        free(outputPLTEStream);
    }	
    else
    {        
        if(pngData.interlace_method == 1)
        {
            
            memset(defiltered, 0, pngData.height * pngData.width * bpp);
            int adam7_offset = 0;  
            int adam7_offsets[7] = {0, 52, 52, 100, 200, 392, 784};

            for(int i = 0; i < 7; i++)
            {
                adam7_offset += adam7_offsets[i];
                defilter_adam7(defiltered, outputStream + adam7_offset, pngData, i);
                LOG(glog, INFO, "adam7 pass %d complete\n",i + 1);
            }           
        }
        else{
            defilter(defiltered , outputStream, pngData.height, bpp * pngData.width + 1, bpp);
        }
    }
    
    LOG(glog, DEBUG, "Writing to file\n");
	FILE *log = fopen("outputlog.ppm" , "w");
	fprintf(log, "P6\n%d %d\n%d\n", pngData.width, pngData.height, (1 << pngData.bit_depth) - 1);

	for(int i = 0; i < (scanwidth - 1) * pngData.height; i++)
    {
        // remove alpha bytes
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
        
    LOG(glog, INFO, "DONE\n");
    
	fclose(log);
	destroy_logger(glog);

    free(outputStream);
	free(pngBuffer);
	free(defiltered);
	free(dataStream);
	return 0;
	
}
