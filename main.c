#include "png.h"
#include "bitstream.h"
#include "huffman.h"
#include <unistd.h>

bool t_validatePNG(FILE *fp);
uint32_t t_readChunkLength(FILE *fp);
void t_readFile(FILE *fp, unsigned char* buffer, uint32_t length);
node *construct_huffman(uint32_t *codes, uint32_t length);
node *construct_first_huffman(uint32_t *codes, uint32_t length);
char* t_readHeader(FILE *fp);
uint32_t t_getCRC(FILE *fp);
void printbits(uint8_t target);
void print32bits(uint32_t target);

typedef struct{
	
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
	
}t_pngMetaData;

typedef struct{
    uint8_t  compression_method;
    uint8_t  compression_info;
    uint8_t  fcheck;
    uint8_t  fdict; 
    uint8_t  flevel;
}t_zlibMetaData;


int main(int argc, char **argv){

	char filename[512];
	if(argc > 1)
	{
		strncpy(filename, argv[1], strlen(argv[1]));
	}
	else
	{	
		strncpy(filename, "res/sample.png", 15);
		
	}
	
	FILE *fp = fopen(filename,"rb");
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
	
	bitstream_t b;
	create_bitstream(&b, dataStream, dataLength);

	uint8_t z_compression_method = read_bits(&b, 4);
	uint32_t z_compression_info = 0x01 << (read_bits(&b, 4) + 0x08);
	uint8_t z_fcheck = read_bits(&b, 5);
	uint8_t z_fdict = read_bits(&b, 1);
	uint8_t z_flevel = read_bits(&b, 2); 
	uint8_t z_final = read_bits(&b, 1);
	uint8_t z_compression_type = read_bits(&b, 2);
	uint32_t z_num_codes_literal = 257 + read_bits(&b, 5);
    uint8_t z_num_codes_distance = 0x01 + read_bits(&b, 5);
    uint8_t z_num_codes_huffman = 0x04 + read_bits(&b, 4);
	
	printf("compression_method %d\n", z_compression_method);
	printf("compression info %d\n", z_compression_info);
	printf("fcheck %d\n", z_fcheck);
	printf("fdict %d\n", z_fdict);
	printf("flevel %d\n", z_flevel);
	printf("final block %d\n", z_final);
	printf("compression type %d\n", z_compression_type);
	printf("literal codes %d\n", z_num_codes_literal);
	printf("distance codes %d\n", z_num_codes_distance);
	printf("huffman codes %d\n", z_num_codes_huffman);

	uint32_t codedict[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};	
	uint32_t codes[19] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	for(int i = 0; i < z_num_codes_huffman; i++)
	{
		codes[i] = read_bits(&b, 3);
		printf("num %d code %d\n",codedict[i], codes[i]);
	}

	node *huffmantree = construct_first_huffman(codes, z_num_codes_huffman);

	uint32_t lencode;
	uint32_t len_buffer;
	uint32_t code;
	uint8_t runlen;
	uint32_t extra = 0;
	uint32_t *z_codes_literal = (uint32_t *) malloc(z_num_codes_literal * sizeof(uint32_t));
	uint32_t *z_codes_distance = (uint32_t *) malloc(z_num_codes_distance * sizeof(uint32_t));
        uint32_t *z_codes = (uint32_t *) malloc((z_num_codes_distance + z_num_codes_literal) * sizeof(uint32_t));
	
        for(int i = 0; i < z_num_codes_literal + z_num_codes_distance; i++)
	{
		runlen = 1;
		lencode = read_bits(&b, 1);
		code = searchCode(huffmantree, lencode, runlen);

		while(code == 0xFFFFFFFF)
		{
			runlen++;
			len_buffer = read_bits(&b, 1);
			lencode = (lencode << 0x01) + len_buffer;
		
//                        printbits(lencode);
                        code = searchCode(huffmantree, lencode, runlen);
		}
                printbits(lencode); 
                z_codes[i] = code;

                if(code >= 16){
                if(code == 16)
                {
                    extra = read_bits(&b, 2);
                    for(int j = 0; j < 3 + extra; j++)
                        {
                            z_codes[i + j] = z_codes[i - 1];
                        }
                        i += extra + 3 - 1;
                }
                else if(code == 17)
                {
                    extra = read_bits(&b, 3);
                    for(int j = 0; j < 3 + extra; j++)
                    {
                        z_codes[i + j] = 0;
                    }
                    i += extra + 3 - 1;
                }
                else if(code == 18)
                {
                    extra = (read_bits(&b, 7));

                    for(int j = 0; j < 10  + extra; j++)
                    {
                        z_codes[i + j] = 0;
                    }
                    i += extra + 10 - 1;
                }

                }
printf("i %d\n", i);
	}

for(int i = 0; i < z_num_codes_literal + z_num_codes_distance; i++)
{
    printf("index: %d code: %d\n",i, z_codes[i]);
}

printf("Got here\n");
        node *len_huffman = construct_huffman(z_codes, z_num_codes_distance + z_num_codes_literal);
        uint32_t count = 0;
	uint32_t newcode;
	while(!z_quit)
	{			
		runlen = 1;
		lencode = read_bits(&b, 1);
		newcode = searchCode(len_huffman, lencode, runlen);
		while(newcode == 0xFFFFFFFF && runlen <= z_num_codes_huffman)
		{
			
			runlen++;
			len_buffer = read_bits(&b, 1);
			lencode = (lencode << 0x01) + len_buffer;
			newcode = searchCode(len_huffman, lencode, runlen);
                        
                //       print32bits(lencode);	
                
		}		
count++;
		

                printf("count: %d length: %d code: %d val: %d\n", count, runlen, lencode, newcode);
                usleep(10 * 1000);

		if(newcode == 256)
		{
			z_quit = true;
		}
                else if(newcode == 0xFFFFFFFF)
                {
                   printf("oh no\n");
                
                print32bits(lencode); 
                return 0;
                }

                else if (newcode > 256)
                {

                    //tidy up this mess
                 int btr = 0;
                 if(newcode > 264)
                     btr++;
                 if(newcode > 268)
                     btr++;
                 if(newcode > 272)
                     btr++;
                 if(newcode > 276)
                     btr++;
                if(newcode > 280)
                    btr++;
                if(newcode > 284)
                    btr = 0;
                if(newcode > 288)
                    btr++;
                if(newcode > 290)
                    btr++;
                if(newcode > 292)
                    btr++;
                if(newcode > 294)
                    btr++;
                if(newcode > 296)
   btr++;            
               if(newcode > 298)
                   btr++;
              if(newcode > 300)
                    btr++;
              if(newcode > 302)
                    btr++;
              if(newcode > 304)
                  btr++;
            if(newcode > 306)
                btr++;
            if(newcode > 308)
                btr++;
            if(newcode > 310)
                btr++;
            if(newcode > 312)
                btr++;
               if(newcode > 314)
                   btr++;
            if(newcode > 316)
                btr++;

                    printf("woo %d\n",btr);
                   usleep(1000 *250); 
                 if(btr > 0)
                     print32bits(read_bits(&b, btr));

                }


		//usleep(500 * 1000);

	}
	//traverse(len_huffman);

	delete_bitstream(&b);
	free(z_codes_literal);
	free(z_codes_distance);

	getchar();
	
	return 0;
}


node *construct_huffman(uint32_t *codes, uint32_t length)
{

	#define MAX_CODES 19
	
	
	node *huffman_tree = createNode();
	uint32_t bl_count[MAX_CODES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int next_code[MAX_CODES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	// can have a code of zero so this should prevent that
	
	int *tree = (int *) malloc(length * sizeof(int));
	memset(tree, -1, length);

	for(int i = 0; i<length; i++)
	{
		bl_count[codes[i]]++;
	}

	bl_count[0] = 0;
	int code = 0;

	for(int bits = 1; bits < MAX_CODES; bits++)
	{
		code = (code + bl_count[bits - 1]) << 1; 
		next_code[bits] = code;
	}

	for(int i = 0; i < MAX_CODES; i++)
	{
		printf("bl_count[%d] %d\n",i, bl_count[i]);	
	}

    for(int i = 0; i < MAX_CODES; i++)
	{		
		printf("next_code[%d] %d\n",i, next_code[i]);
	}

	for(int n = 0; n < length; n++)
	{
		uint32_t len = codes[n];
		
		if(len != 0 && len != 0xFFFFFFFF)
		{						
			tree[n] = next_code[len];			
			insertCode(huffman_tree, next_code[len], len, n); 
			next_code[len]++;
		}
	}

	for(int i = 0; i < length; i++)
	{		
		if(tree[i] != -1){
			//printf("tree[%d] %d\n",i, tree[i]);
		}		
	}

	free(tree);
	return huffman_tree;

}


node *construct_first_huffman(uint32_t *codes, uint32_t length)
{

	#define MAX_CODES 19
	
	node *huffmantree = createNode();

	uint32_t codedict[MAX_CODES] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};	
	uint8_t bl_count[8] = {0,0,0,0,0,0,0,0};
	int next_code[8] = {0,0,0,0,0,0,0,0};

printf("%d\n", codedict[0]);
	// can have a code of zero so this should prevent that
	uint32_t tree[MAX_CODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

	for(int i = 0; i<length; i++)
	{
		bl_count[codes[i]]++;
	}

	bl_count[0] = 0;
	int code = 0;


printf("%d\n", codedict[0]);
	for(int bits = 1; bits < 8; bits++)
	{
	
                code = (code + bl_count[bits - 1]) << 1; 
		next_code[bits] = code;
	
printf("in loop %d\n", codedict[0]);
        }

printf("%d\n", codedict[0]);
	for(int i = 0; i < 8; i++)
	{
		printf("bl_count[%d] %d\n",i, bl_count[i]);	
	}

printf("%d\n", codedict[0]);
    for(int i = 0; i < 8; i++)
	{		
		printf("next_code[%d] %d\n",i, next_code[i]);
	}


printf("%d\n", codedict[0]);
	for(int n = 0; n <= length; n++)
	{
		int len = codes[n];
		
		if(len != 0 && len != 0xFFFFFFFF)
		{						
			tree[n] = next_code[len];			
			insertCode(huffmantree, next_code[len], len, codedict[n]); 
			next_code[len]++;
		}
	}

printf("%d\n", codedict[0]);
	for(int i = 0; i < length; i++)
	{		
		if(tree[i] != -1){
			printf("%d tree[%d] %d\n",i, codedict[i], tree[i]);			
			//output[codedict[i]] = tree[i];
		}		
	}

	return huffmantree;

}

void read_codes(unsigned char *buffer, uint32_t length)
{

	uint32_t count = 0;
	uint8_t data = 0;

	// first byte is already 1 bit offset
	uint16_t mask = 0x0E;
	uint8_t shift = 0x01;

	data = (uint8_t) *(buffer++);
	printf("count: %d byte: %02x\n", count, data);			
	count++;

	uint8_t codes[length];
	int codenum = 0;	

	while(codenum < length)
	{			
		
		if(((mask & 0xFF00) != 0) && ((mask & 0x00FF) != 0))
		// code has partially rolled over
		{			
			
			uint8_t code = 0;
			
			// lower byte
			code = (data & (mask & 0xFF)) >> shift;
			

			data =(uint8_t) *(buffer++);
			printf("count: %d byte: %02x\n", count, data);			
			count++;

			// higher byte
			code += (data & (mask >> 0x08)) << (0x08 - shift);
			codes[codenum++] = code;			


			shift = (mask >> 0x08 == 0x01) ? 0x01 : 0x02;
			mask = 0x07 << shift;	
			printf("PART %d\n",code);		
		}

		else if(((mask & 0xFF00) == 0x0700) && ((mask & 0x00FF) == 0x00))
		// code has completely rolled over
		{			
			mask = 0x07;
			shift = 0;

			data = (uint8_t) *(buffer++);		
			printf("count: %d byte: %02x\n", count, data);	
			count++;
			
			codes[codenum++] = (data & mask) >> shift;

			mask = mask << 0x03;
			shift += 3;
			printf("ROLL %d\n",codes[codenum - 1]);
		}
		else
		// code has not rolled over
		{			
			
			codes[codenum++] = (data & mask) >> shift;	

			mask = mask << 0x03;
			shift += 3;
			printf("NUN %d\n",codes[codenum - 1]);
		}
		
	}

	uint8_t codedict[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
	for(int i=0; i < codenum; i++)
	{
		printf("Code %d: %d\n",codedict[i], codes[i]);
	}	
	
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

void printbits(uint8_t target)
{
	char str[9] = "";

	for(int i = 0; i < 8; i++)
	{
		
		int nextbit = (target >> (7 - i)) & 0x01;
		str[i] = nextbit ? '1' : '0';		
		
	}

	str[8] = '\0';
	printf("%s\n", str);

}

void print32bits(uint32_t target)
{

    char str[33] = "";

    for(int i = 0; i < 32; i++)
    {
            
            int nextbit = (target >> (31 - i)) & 0x01;
            str[i] = nextbit ? '1' : '0';		
            
    }

    str[32] = '\0';
    printf("%s\n", str);
}
