#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include "inflate.h"
#include "logging.h"
#include "gzip.h"



void print8bits(uint8_t target);
void print32bits(uint32_t target);
void printNbits(uint32_t target, int N);
uint32_t reverse_bits(uint32_t target);

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

        unsigned char *pngBuffer, *output; 
        uint32_t offset = 8;
 	FILE *fp = fopen(filename, "rb");

	if (!validatePNG(fp))
	{
            LOG(ERROR, "Could not validate png file\n");
            return -1;
	}

	fclose(fp);

	readFileToBuffer(filename,&pngBuffer);

	pngMetaData pngData; 
	char chunkType[5] = "1234";
	
	bool png_quit = false; 
	uint32_t chunkLength = 0;

	uint8_t *dataStream = (uint8_t *)malloc(sizeof(uint8_t *));
	uint8_t *chunkBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
	uint8_t *crcBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
	uint32_t dataLength = 0;
	
	while(!png_quit){
		chunkLength = getChunkLength(pngBuffer + offset);
		printf("chunk length %d\n", chunkLength);
		offset += 4; 

		getChunkType(pngBuffer + offset, chunkType);
		
		offset += 4; 
		printf("type: %s\n", chunkType);

		chunkBuffer = realloc(chunkBuffer, chunkLength * sizeof(uint8_t));
		getChunkData(pngBuffer + offset, chunkLength, chunkBuffer);

		if(strncmp(chunkType, "IHDR", 4) == 0){
			pngData = processHeader(chunkBuffer);
		}
		else if(strncmp(chunkType, "IDAT", 4) == 0)
		{
			dataLength += chunkLength;
			dataStream = realloc(dataStream, dataLength * sizeof(uint8_t));
			memcpy(dataStream + (dataLength - chunkLength), chunkBuffer, chunkLength);
			//getChunkData(pngBuffer + offset, chunkLength, dataStream + (dataLength - chunkLength));
		}
		else if(strncmp(chunkType, "IEND", 4) == 0)
		{
			png_quit = true;
		}

		offset += chunkLength;

		uint32_t crc = getCRC(pngBuffer + offset);
		printf("CRC: 0x%x\n", crc);

		crcBuffer = realloc(crcBuffer, (chunkLength + 4) * sizeof(uint8_t));
		
		memcpy(crcBuffer, chunkType, 4);
		memcpy(crcBuffer + 4, chunkBuffer, chunkLength);		

		if(validateCRC(crcBuffer, chunkLength + 4, crc))
		{
			LOG(DEBUG, "OK CRC\n");
		}
		else
		{
			LOG(ERROR, "Invalid CRC\n");
			exit(1);
		}

		offset += 4;
	}

	printf("datalength %d\n", dataLength);    

	//zlib section

       
	int bpp = pngData.bit_depth / 8;
	
	switch(pngData.color_type)
	{
		case 0:
			// grayscale and no alpha channel. Do not need to modify bpp
		break;

		case 2:
			// rgb triplet
			bpp *= 3;
		break;
		case 3:
			// using a pallete. This is currently not supported
		break;
		
		case 4:
			// grayscale with an alpha channel
			bpp *= 2;
		break;
		case 6:
			// rgb with and alpha channel
			bpp *= 4;
		break;

		default:
		break;
	}

	int scanwidth = pngData.width * bpp + 1;

        uint32_t uncompressed_size = (scanwidth) * pngData.height;
	uint8_t *outputStream = malloc(uncompressed_size * sizeof(uint8_t));
	uint32_t outputLength = 0;

	bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	zlibMetaData z_data = z_processHeader(&b);
	
	uint32_t len = z_inflate(&b, outputStream);
	printf("len %d %d\n", len, uncompressed_size);
        printf("bpp %d\n", bpp);

	uint32_t adler = z_readADLER32(&b);
	uint32_t adler_computed = z_ADLER32(outputStream, len);

	if(adler != adler_computed)
	{
		printf("BAD ADLER32 0x%x != 0x%x\n", adler, adler_computed);
	}

	delete_bitstream(&b);
	

	int filterType = 0;
	uint8_t *prevScanline = malloc(scanwidth - 1);
	uint8_t *currentScanline = malloc(scanwidth - 1);
        memset(prevScanline, 0, scanwidth - 1);
	FILE *ofp = fopen("outputlog.ppm", "w");
	FILE *ofpr = fopen("decompressed.ppm", "w");

        fprintf(ofp, "P6\n128 68\n255\n");
        fprintf(ofpr, "P6\n128 68\n255\n");

        for(int i = 0; i < uncompressed_size; i++)
        {
            fputc( outputStream[i], ofpr);
        }
        fclose(ofpr);

       // for(int i = 0; i < pngData.height - pngData.height + 1; i++)

        for(int i = 0; i < pngData.height ; i++)
	{
		filterType = outputStream[i * scanwidth];
		
		switch(filterType)
		{
			case 0:
				LOG(DEBUG, "None\n");
				for(int j = 1; j < scanwidth; j++)
				{
					currentScanline[j - 1] = outputStream[i * scanwidth + j];
					fputc(currentScanline[j - 1], ofp);
				}
				break;
			case 1:
				LOG(DEBUG, "Sub\n");
				break;
			case 2:
				LOG(DEBUG, "Up\n");
				break;
			case 3:
				LOG(DEBUG, "Average\n");
				break;
			case 4:
				LOG(DEBUG, "Paeth\n");
				for(int j = 1; j < scanwidth; j++)
					{
						// paeth algorithm
						
						uint8_t paeth_left, paeth_top_left, paeth_top;
						
						if(i == 0)
						{
							paeth_top = 0;
							paeth_top_left = 0;
						}
						else
						{				
							paeth_top = prevScanline[j - 1];
							paeth_top_left = prevScanline[j - 1 - bpp];
						}

						if(j < (bpp + 1))
						{
							paeth_left = 0;
							paeth_top_left = 0;
						}
						else
						{
                                                        paeth_left = currentScanline[j - 1 - bpp];
						}

						int res = paeth_left + paeth_top - paeth_top_left;
						int ret = 0;
						int reconstructed = 0;
						
						int res_left =     (res - paeth_left);
						int res_top =      (res - paeth_top);
						int res_top_left = (res - paeth_top_left);

						if((res_left <= res_top) && (res_left <= res_top_left))
						{
							ret =  paeth_left;
						}
						else if(res_top <= res_top_left)
						{
							ret = paeth_top ;
						}
						else
						{
							ret = paeth_top_left ;
						}	            

							reconstructed =  (outputStream[i * scanwidth + j] + ret) ;
                                                        currentScanline[j - 1] = reconstructed &
                                                        0xFF;
							//fprintf(ofp, "%x", reconstructed);
                                                        fputc(reconstructed, ofp);
						}			
						
				break;
			default:
				LOG(ERROR, "Something is broken %d\n", filterType);
				break;
		}

		memcpy(prevScanline, currentScanline, scanwidth - 1);		
                /*for(int l = 0; l < scanwidth; l++)
                {
                    printf("0x%02x ", currentScanline[l]);
                }
                printf("\n");
                */
        }


	fclose(ofp);

        // inflate is complete now need to undo the filter operations


	free(outputStream);	
	return 0;

	// relax this is temporary
exit:	

	free(dataStream);
	free(pngBuffer);
	delete_bitstream(&b);
	getchar();

	return 1;

}

// some debugging utility functions
void printNbits(uint32_t target, int N)
{
	// 32 bit max but can print a variable number
	char str[33] = "";

	for (int i = 0; i < N; i++)
	{
		int nextbit = (target >> (N - 1 - i)) & 0x01;
		str[i] = nextbit ? '1' : '0';
	}

	str[N] = '\0';
	LOG(DEBUG, "%s\n", str);
}

void print8bits(uint8_t target)
{
	char str[9] = "";

	for (int i = 0; i < 8; i++)
	{

		int nextbit = (target >> (7 - i)) & 0x01;
		str[i] = nextbit ? '1' : '0';
	}

	str[8] = '\0';
	LOG(DEBUG, "%s\n", str);
}

void print32bits(uint32_t target)
{
	char str[33] = "";

	for (int i = 0; i < 32; i++)
	{
		int nextbit = (target >> (31 - i)) & 0x01;
		str[i] = nextbit ? '1' : '0';
	}

	str[32] = '\0';
	LOG(DEBUG, "%s\n", str);
}

uint32_t reverse_bits(uint32_t target)
{

	uint32_t res = 0;
	
	for(int i = 0; i < 32; i++)
	{		
		res += ((target >> i) & 0x01)  << (31 - i);
	}

	return res;
}
