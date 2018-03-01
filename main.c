#include <stdio.h>
#include <SDL2/SDL.h>
#include "png.h"

int main(int argc, char **argv)
{
	
	FILE *fp = fopen("../res/sample.png","rb");
	
	if(fp == NULL){
		return NULL;
	}
	
	char *buffer;
	
	buffer = (char*) malloc(sizeof(char)*100);
	
	for(int i = 0; i < 8; i++){
		buffer[i] = fgetc(fp);
	}
	
	PNGMetaData p;
	uint32_t chunkLen;
	char *chunkType;
	char *chunkData;
	uint32_t chunkCRC;
	
	chunkLen = getChunkLength(fp);
	printf("%d\n",chunkLen);
	
	chunkType = malloc(5*sizeof(char));
	chunkData = malloc(chunkLen*sizeof(char) + sizeof(char));
	
	getChunkType(fp,chunkType);
	printf("%s\n",chunkType);
	
	getChunkData(fp,chunkLen,chunkData);
	printf("%s\n",chunkData);
	
	chunkCRC = getCRC(fp);
	printf("%u\n", chunkCRC);
	
	p = processHeader(chunkData);
	
	getchar();
	
	fclose(fp);
	free (chunkType);
	free (chunkData);
	
	return 0;
}