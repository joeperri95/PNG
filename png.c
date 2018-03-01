#include "png.h"

bool validatePNG(FILE *fp){
	
	char buffer[8];
	int codes[] = {137,80,78,71,13,10,26,10};
	
	for(int i = 0;i < 8; i++){
		buffer[i] = fgetc(fp);
		if((int)(unsigned char)buffer[i] != codes[i]){
			return false;
		}
	}
	
	return true;
}

pngMetaData processHeader(char *chunkData){
	
	pngMetaData result;
	
	memcpy(result.width,4,chunkData);
	memcpy(result.height,4,chunkData);
	memcpy(result.bit_depth,1,chunkData);
	memcpy(result.color_type,1,chunkData);
	memcpy(result.compression_method,1,chunkData);
	memcpy(result.filter_method,1,chunkData);
	memcpy(result.interlace_method,1,chunkData);
	
	return result;
}

uint32_t getChunkLength(FILE *fp){
	
	unsigned char buffer[4];
	uint32_t result;
	
	for(int i = 0; i < 4; i++){
		buffer[i] = fgetc(fp);
	}
	
	result = ((int)buffer[0] << 24) + ((int)buffer[1] << 16) + ((int)buffer[2] << 8) + ((int)buffer[3]);
	
	return result;
}

//store string containing chunk type in str
void getChunkType(FILE *fp,char *str){
	
	for(int i = 0; i < 4; i++){
		str[i] = fgetc(fp);
	}
	
	str[4] = '\0';
	
	return;
}

void getChunkData(FILE *fp, uint32_t chunkLength, char *chunkData){
	
	for(uint32_t i = 0; i < chunkLength; i++){
		chunkData[i] = fgetc(fp);
	}
	
	chunkData[chunkLength] = '\0';
	
	return;
}

uint32_t getCRC(FILE *fp){
	
	unsigned char buffer[4];
	uint32_t result;
	
	for(int i = 0; i < 4; i++){
		buffer[i] = fgetc(fp);
	}
	
	result = ((int)buffer[0] << 24) + ((int)buffer[1] << 16) + ((int)buffer[2] << 8) + ((int)buffer[3]);
	
	return result;
}