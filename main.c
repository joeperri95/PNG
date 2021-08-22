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
	unsigned char *dataStream = (unsigned char *)malloc(sizeof(unsigned char *));
	uint32_t dataLength = 0;
	
	while(!png_quit){
		chunkLength = getChunkLength(pngBuffer + offset);
		printf("chunk length %d\n", chunkLength);
		offset += 4; 

		getChunkType(pngBuffer + offset, chunkType);
		
		offset += 4; 
		printf("type: %s\n", chunkType);

		if(strncmp(chunkType, "IHDR", 4) == 0){
			pngData = processHeader(pngBuffer + offset);
		}
		else if(strncmp(chunkType, "IDAT", 4) == 0)
		{
			dataLength += chunkLength;
			dataStream = realloc(dataStream, dataLength * sizeof(unsigned char));
			getChunkData(pngBuffer + offset, chunkLength, dataStream + (dataLength - chunkLength));
		}
		else if(strncmp(chunkType, "IEND", 4) == 0)
		{
			png_quit = true;
		}
		offset += chunkLength;

		uint64_t crc = getCRC(pngBuffer + offset);
		printf("CRC: %u\n", crc);

		if(validateCRC)

		offset += 4;
	}

	printf("datalength %d\n", dataLength);    

	//zlib section

	// bytes per pixel. This may not be perfect
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
			// grayscale with an alpha channgel
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


    uint32_t uncompressed_size = (pngData.width + 1) * pngData.height * 8;
	uint8_t *outputStream = malloc(uncompressed_size * sizeof(uint8_t));
	uint32_t outputLength = 0;

	bool z_quit = false;
    uint8_t z_final = 0; 
	bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	zlibMetaData z_data = z_processHeader(&b);
	
	z_inflate(&b, outputStream);
	
	z_readADLER32(&b);

	delete_bitstream(&b);
	


	int filterType = 0;

	FILE *ofp = fopen("outputlog.csv", "w");
	for(int i = 0; i < pngData.height; i++)
	{
		filterType = outputStream[i * scanwidth];
		
		// switch(filterType)
		// {
		// 	case 0:
		// 		printf("None\n");
		// 		break;
		// 	case 1:
		// 		printf("Sub\n");
		// 		break;
		// 	case 2:
		// 		printf("Up\n");
		// 		break;
		// 	case 3:
		// 		printf("Average\n");
		// 		break;
		// 	case 4:
		// 		printf("Paeth\n");
		// 		break;
		// 	default:
		// 		printf("Something is broken %d\n", filterType);
				
		// 		break;
		// }
		
		for(int j = 1; j < scanwidth; j++)
		{
			// paeth algorithm
			
			int paeth_left, paeth_top_left, paeth_top;
			
			if(i == 0)
			{
				paeth_top = 0;
				paeth_top_left = 0;
			}
			else
			{
				paeth_top = outputStream[(i - 1) * scanwidth + j];
				paeth_top_left = outputStream[(i - 1) * scanwidth + j - 3];
			}

			if(j <= 4)
			{
				paeth_left = 0;
				paeth_top_left = 0;
			}
			else
			{
				paeth_left = outputStream[i * scanwidth + j - 3];
			}

			int res = paeth_left + paeth_top - paeth_top_left;
			int ret = 0;
			
			int res_left =     (res - paeth_left) & 0xFF;
			int res_top =      (res - paeth_top) & 0xFF;
			int res_top_left = (res - paeth_top_left) & 0xFF;

			if((res_left <= res_top) && (res_left <= res_top_left))
			{
				ret =  res_left;
			}
			else if(res_top <= res_top_left)
			{
				ret = res_top;
			}
			else
			{
				ret = res_top_left;
			}
	                fputc(' ', ofp); 
                        fputc(ret, ofp);

                }		
	fputc('\n', ofp);	
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
