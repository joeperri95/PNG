#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include "inflate.h"
#include "logging.h"
#include "gzip.h"
#include "checksum.h"
#include "image.h"


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


        // This could be sped up 
        // Should also be moved to the lib
	while(!png_quit){
		chunkLength = getChunkLength(pngBuffer + offset);
	        LOG(DEBUG, "chunk length %d\n", chunkLength);
		offset += 4; 

		getChunkType(pngBuffer + offset, chunkType);
		
		offset += 4;
		LOG(INFO, "Chunk type: %s\n", chunkType);

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
		}
                else if(strncmp(chunkType, "sRGB", 4) == 0)
                {
                    LOG(WARNING, "SRGB Not implemented\n");
                }
                else if(strncmp(chunkType, "iTXt", 4) == 0)
                {
                    LOG(WARNING, "Not yet complete\n");
                    // Not sure if deflate or zlib
                    uint32_t offset = 0;
                    int keyLength = strlen(chunkBuffer);
                    offset += keyLength + 1;
                   
                    bool compression = *(chunkBuffer + offset++);
                    uint8_t compressionMethod = *(chunkBuffer + offset++);
                    char *language;
                   
                    // if there is a language tag include it.
                    if(strlen(chunkBuffer + offset) != 0){
                        strcpy(language, chunkBuffer + offset);
                        offset += strlen(language);
                    }
                    else
                    {
                        offset++;
                    }
                    
                    // print the keyword first
                    LOG(INFO, "%s: ", chunkBuffer);
                    
                    // There may be null characters in the string
                    for(int i = offset; i < chunkLength; i++)
                    {
                        printf("%c", chunkBuffer[i]);
                    }
                    printf("\n");

                }
                else if(strncmp(chunkType, "tEXt", 4) == 0)
                {
                    int keyLength = strlen(chunkBuffer);
                    LOG(INFO, "%s: %.*s\n", chunkBuffer, chunkLength - keyLength - 1, chunkBuffer + keyLength + 1);
                }
                else if(strncmp(chunkType, "zTXt", 4) == 0)
                {
                    LOG(WARNING, "Not yet complete\n");

                    // Not sure if deflate or zlib
                    uint32_t offset = 0;
                    int keyLength = strlen(chunkBuffer);
                    offset += keyLength + 1;
                    
                    // Compression method but currently only deflate
                    uint8_t compressionMethod = *(chunkBuffer + offset++); 
                    uint8_t *outputMessage = malloc(1024 *sizeof(uint8_t)); 
                    bitstream_t message;

                    create_bitstream(&message, chunkBuffer + offset, chunkLength - offset); 
                    z_inflate(&message, outputMessage);

                    LOG(INFO, "%s: %s\n", chunkBuffer, outputMessage);

                    delete_bitstream(&message);
                    free(outputMessage);
                }
		else if(strncmp(chunkType, "IEND", 4) == 0)
		{
			png_quit = true;
		}

		offset += chunkLength;

		uint32_t crc = getCRC(pngBuffer + offset);

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
                        LOG(ERROR, "Pallete chunks not supported\n");
                        exit(1);
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

	bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	zlibMetaData z_data = z_processHeader(&b);
	
	uint32_t len = z_inflate(&b, outputStream);

        if(len != uncompressed_size)
        {
                LOG(WARNING, "inflate size wrong\n");
        }

	uint32_t adler = z_readADLER32(&b);
	uint32_t adler_computed = ADLER32(outputStream, len);

        if(b.byte_offset != b.length)
        {
                LOG(WARNING, "Extra bytes in bitstream\n");
        }

        if(b.bit_offset != 0)
        {
                LOG(WARNING, "Bitstream has offset\n");
        }

	if(adler != adler_computed)
	{
		LOG( WARNING, "BAD ADLER32 0x%x != 0x%x\n", adler, adler_computed);
	}
        else
        {
		LOG(INFO , "GOOD ADLER32 0x%x\n",adler);
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
