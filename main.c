#include <stdio.h>
#include <SDL2/SDL.h>
#include "png.h"

bool t_validatePNG(FILE *fp);
uint32_t t_readChunkLength(FILE *fp);
void t_readFile(FILE *fp, unsigned char* buffer, uint32_t length);
char* t_readHeader(FILE *fp);
uint32_t t_getCRC(FILE *fp);

typedef struct{
	
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
	
}t_pngMetaData;



int main(int argc, char **argv){
	
	FILE *fp = fopen("../res/sample.png","rb");
	bool png_quit = false;
	unsigned char* buffer;
	unsigned char* dataStream = (unsigned char*) malloc(sizeof(unsigned char*));
	uint32_t dataLength = 0;
	uint32_t length;
	uint32_t png_w, png_h;
	unsigned char png_bitDepth; 
	unsigned char png_colorType;
	unsigned char png_compressionMethod;
	unsigned char png_filterMethod;
	unsigned char png_interlaceMethod;
	
	if(t_validatePNG(fp)){
		puts("success");
	}
	else{
		puts("uh oh");
		png_quit = true;
	}
	
	while(!png_quit){
		length = t_readChunkLength(fp);
		
		printf("length: %" PRIu32 "\n", length);
		
		char *chunkType = t_readHeader(fp);
		printf("%s\n", chunkType);
		
		buffer = (unsigned char *) malloc(sizeof(unsigned char) * length);
		
		t_readFile(fp, buffer, length);
		
		if(strcmp(chunkType, "IHDR") == 0){			
			
			png_w = *buffer << 24;
			buffer++;
			
			png_w += *buffer << 16;
			buffer++;
			
			png_w += *buffer << 8;
			buffer++;
			
			png_w += *buffer;
			buffer++;
			
			png_h = *buffer << 24;
			buffer++;
			
			png_h += *buffer << 16;
			buffer++;
			
			png_h += *buffer << 8;
			buffer++;
			
			png_h+= *buffer;
			buffer++;
			
			png_bitDepth = *buffer;
			buffer++;
			
			png_colorType = *buffer;
			buffer++;
			
			png_compressionMethod = *buffer;
			buffer++;
			
			png_filterMethod = *buffer;
			buffer++;
			
			png_interlaceMethod = *buffer;
			buffer++;
			
			printf("Width %" PRIu32 "\n", png_w);
			printf("Height %" PRIu32 "\n", png_h);
			printf("Bit Depth %d\n", png_bitDepth);
			printf("Color Type %d\n", png_colorType);
			printf("Compression Method %d\n", png_compressionMethod);
			printf("Filter Method %d\n", png_filterMethod);
			printf("Interlace Method %d\n", png_interlaceMethod);
		}
		else if(strcmp(chunkType, "IDAT") == 0){
			dataLength += length;
			dataStream = (unsigned char*) realloc(dataStream, sizeof(unsigned char*) * dataLength);
			
			//rewind to beginning of memory to be written
			dataStream += dataLength - length;
			
			for(int i = 0; i < length; i++){
				
				*dataStream = *buffer;
				
				dataStream++;
				buffer++;
			}
			
			dataStream -= dataLength;
		}
		
		else if(strcmp(chunkType, "IEND") == 0){
			png_quit = true;
		}
		
		
		putchar('\n');
		
		uint32_t CRC = t_getCRC(fp);
		
		printf("CRC: %" PRIu32 "\n", CRC);
		printf("datalen %" PRIu32 "\n", dataLength);
	}
	
	fclose(fp);
	
	//zlib section
	bool z_quit = false;
	uint32_t counter = 0;
	unsigned char z_cmf;
	unsigned char z_flg;
	uint32_t z_fdict;
	
	//Handle cmf and flag
	z_cmf = *dataStream;
	dataStream++;
	counter++;
	
	z_flg = *dataStream;
	dataStream++;
	counter++;
	
	if(z_flg && 0x10){
		
		z_fdict = *dataStream << 24;
		dataStream++;
		counter++;
		
		z_fdict += *dataStream << 16;
		dataStream++;
		counter++;
		
		z_fdict += *dataStream << 8;
		dataStream++;
		counter++;
		
		z_fdict += *dataStream;
		dataStream++;
		counter++;
	}
	
	unsigned char z_buffer;
	bool z_final = false;	
	int z_compression_type;
	
	
	while(!z_final){
		
		z_buffer = *dataStream;
		dataStream++;
		
		if(z_buffer && BFINAL){
			z_final = true;
		}
		
		z_compression_type = z_buffer && BTYPE;
		
		if(z_compression_type == NO_COMPRESSION){
			//get len and nlen
			printf("no compression\n");
		}
		else if(z_compression_type == FIXED_HUFFMAN){
			printf("fixed huffman\n");
		}
		else if(z_compression_type == DYNAMIC_HUFFMAN){
			printf("dynamic huffman\n");
		}
		else if(z_compression_type == RESERVED_ERROR){
			printf("error\n");
		}
		else{
			printf("super error\n");
		}
		
		
		
		while(!z_quit){
			
			z_buffer = *dataStream;
			dataStream++;
			
			
			z_quit = true;
		}
		
	}
	getchar();
	
	return 0;
}


bool t_validatePNG(FILE *fp){
	
	unsigned char loop;
	unsigned char png_v[8] = {137,80,78,71,13,10,26,10};
	bool res = true;
	
	for(int i = 0; i < 8; i++){
		
		loop = fgetc(fp);
		
		if(loop != png_v[i]){
			res = false;
		}
	}
	
	return res;
}

uint32_t t_readChunkLength(FILE *fp){
	
	unsigned char loop;
	uint32_t res = 0;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 24;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 16;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 8;
	
	loop = fgetc(fp);
	res += (uint32_t) loop;
	
	return res;
}

void t_readFile(FILE *fp, unsigned char* buffer, uint32_t length){
	
	for(uint32_t i = 0; i < length; i++){
		*buffer = fgetc(fp);
		buffer++;
	}
	
	//rewind to start of memory
	buffer -= length;
}

char* t_readHeader(FILE *fp){
	
	char *type = malloc(sizeof(char)*5);
	
	for(int i = 0; i < 4; i++){
		*type = fgetc(fp);
		type++;
	}
	
	//Terminate C string
	*type = '\0';
	
	//Rewind to start of string
	type -= 4;
	
	return type;
}

uint32_t t_getCRC(FILE *fp){
	
	unsigned char loop;
	uint32_t res = 0;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 24;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 16;
	
	loop = fgetc(fp);
	res += (uint32_t) loop << 8;
	
	loop = fgetc(fp);
	res += (uint32_t) loop;
	
	return res;
}

