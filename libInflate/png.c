#include "png.h"

pngMetaData process_PNG_header(uint8_t *buffer)
{
    pngMetaData pngData;

    int offset = 0;
    pngData.width = *(buffer + offset++) << 24;
    pngData.width += *(buffer + offset++) << 16;
    pngData.width += *(buffer + offset++) << 8;
    pngData.width += *(buffer + offset++);

    pngData.height = *(buffer + offset++) << 24;
    pngData.height += *(buffer + offset++) << 16;
    pngData.height += *(buffer + offset++) << 8;
    pngData.height += *(buffer + offset++);

    pngData.bit_depth = *(buffer + offset++);
    pngData.color_type = *(buffer + offset++);
    pngData.compression_method = *(buffer + offset++);
    pngData.filter_method = *(buffer + offset++);
    pngData.interlace_method = *(buffer + offset++);

    LOG(glog, INFO, "Width %" PRIu32 "\n", pngData.width);
    LOG(glog, INFO, "Height %" PRIu32 "\n", pngData.height);
    LOG(glog, INFO, "Bit Depth %d\n", pngData.bit_depth);
    LOG(glog, INFO, "Color Type %d\n", pngData.color_type);
    LOG(glog, INFO, "Compression Method %d\n", pngData.compression_method);
    LOG(glog, INFO, "Filter Method %d\n", pngData.filter_method);
    LOG(glog, INFO, "Interlace Method %d\n", pngData.interlace_method);

    return pngData;
}


void process_chunks(uint8_t *fileBuffer, uint8_t **outputBuffer, uint32_t *outputLength, pngMetaData *pngData, pallete *plte)
{

    bool png_quit = false;
    
    uint32_t chunkLength = 0;
    uint32_t offset = 0;
    uint32_t dataLength = 0;
    
    uint8_t *chunkBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
    uint8_t *crcBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
    
    //uint8_t *IDATBuffer = (uint8_t *)malloc(sizeof(uint8_t *));
    
    char chunkType[5] = "";    

    while(!png_quit){
		
        chunkLength = get_chunk_length(fileBuffer + offset);
	    LOG(glog, DEBUG, "chunk length %d\n", chunkLength);
		offset += 4; 

		get_chunk_type(fileBuffer + offset, chunkType);
		
		offset += 4;
		LOG(glog, INFO, "Chunk type: %s\n", chunkType);

		chunkBuffer = realloc(chunkBuffer, chunkLength * sizeof(uint8_t));
		get_chunk_data(fileBuffer + offset, chunkLength, chunkBuffer);

        // Need to avoid all of these strncmp calls possibly using a hash function
        // Also need to check order
        if(strncmp(chunkType, "IHDR", 4) == 0){
                *pngData = _process_IHDR(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "IDAT", 4) == 0)
        {
                _process_IDAT(chunkBuffer, chunkLength, outputBuffer, outputLength);
        }
        else if(strncmp(chunkType, "PLTE", 4) == 0)
        {
                _process_PLTE(chunkBuffer, chunkLength, plte);
        }
        else if(strncmp(chunkType, "sRGB", 4) == 0)
        {
            _process_sRGB(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "iTXt", 4) == 0)
        {
            _process_iTXt(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "tEXt", 4) == 0)
        {
            _process_tEXt(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "zTXt", 4) == 0)
        {
            _process_zTXt(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "bKGD", 4) == 0)
        {
            _process_bKGD(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "pHYs", 4) == 0)
        {
            _process_pHYs(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "sBIT", 4) == 0)
        {
            _process_sBIT(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "sPLT", 4) == 0)
        {
            _process_sPLT(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "hIST", 4) == 0)
        {
            _process_hIST(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "tIME", 4) == 0)
        {
            _process_tIME(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "tRNS", 4) == 0)
        {
            _process_tRNS(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "gAMA", 4) == 0)
        {
            _process_gAMA(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "cHRM", 4) == 0)
        {
            _process_cHRM(chunkBuffer, chunkLength);
        }
        else if(strncmp(chunkType, "iCCP", 4) == 0)
        {
            _process_iCCP(chunkBuffer, chunkLength);
        }
		else if(strncmp(chunkType, "IEND", 4) == 0)
		{
			png_quit = true;
		}

		offset += chunkLength;

		uint32_t crc = get_CRC(fileBuffer + offset);

        // copy chunk type and data to crcbuffer
		crcBuffer = realloc(crcBuffer, (chunkLength + 4) * sizeof(uint8_t));		
		memcpy(crcBuffer, chunkType, 4);
		memcpy(crcBuffer + 4, chunkBuffer, chunkLength);		

		if(validate_CRC(crcBuffer, chunkLength + 4, crc))
		{
			LOG(glog, DEBUG, "OK CRC\n");
		}
		else
		{
			LOG(glog, ERROR, "Invalid CRC\n");
			exit(1);
		}

		offset += 4;
	}
}

bool validate_PNG(FILE *fp)
{
    uint8_t loop;
    uint8_t png_v[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    bool res = true;
    
    for (int i = 0; i < 8; i++)
    {
        loop = fgetc(fp);        

        if (loop != png_v[i])
        {
            res = false;
        }
    }

    return res;
}

uint32_t get_chunk_length(uint8_t *buffer)
{

	uint8_t loop;
	uint32_t res = 0;

	loop = *(buffer++);
	res += (uint32_t)loop << 24;

	loop = *(buffer++); 
	res += (uint32_t)loop << 16;
        
        loop = *(buffer++); 
	res += (uint32_t)loop << 8;
        
        loop = *(buffer++); 
	res += (uint32_t)loop;

	return res;
}

void get_chunk_data(uint8_t *fileBuffer, uint32_t chunkLength, uint8_t *chunkBuffer)
{
        memcpy(chunkBuffer, fileBuffer, chunkLength * sizeof(uint8_t)); 
}

void get_chunk_type(uint8_t *buffer, char * str)
{
    
    memcpy(str, buffer, 4 * sizeof(char));
    str[5] = '\0';

}

uint32_t get_CRC(uint8_t *buffer)
{

	uint32_t res = 0;

	res += ((uint32_t) *(buffer)) << 24;
	res += ((uint32_t) *(buffer + 1)) << 16;
	res += ((uint32_t) *(buffer + 2)) << 8;
	res += *(buffer + 3);

	return res;
}

bool validate_CRC(uint8_t* buffer, uint32_t length, uint32_t CRC)
{
    //  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
    // 0001 0000 0100 1100 0001 0001 1101 1011 0111 
    
    uint32_t ret = CRC32(buffer, length);
    return ( ret == CRC);

}

uint8_t get_bytes_per_pixel(pngMetaData params)
{

    uint bpp = params.bit_depth / 8;
	
    switch(params.color_type)
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
                    return 3;
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
                    LOG(glog, ERROR, "Invalid PNG color type\n");
                break;
    }
    
    return bpp;
}

void decode_pallete(uint8_t *outputBuffer, uint8_t* inputBuffer, pngMetaData params, pallete plte)
{
    // Always 3 bytes per pixel with a pallete
    const int bpp = 3;

    for(int i = 0; i < params.height; i++)
    {
        outputBuffer[i *  (1 + bpp * params.width)] =  inputBuffer[i * (1 + params.width)];  
        LOG(glog, DEBUG_3 + 1, "Row: %d Offs: %d filter type %d\n", i, i * (1 + bpp *params.width) ,outputBuffer[i *  (1 + bpp * params.width)]);
        
        for(int j = 0; j < params.width; j++)
        {
            
            int outputIndex = ((params.width) * bpp + 1) * i + 3 * j; 
            int index = inputBuffer[(1 + params.width) * i + j + 1];
            pallete_pixel p = plte.buffer[index];

            outputBuffer[outputIndex + 1] = p.R;
            outputBuffer[outputIndex + 2] = p.G;
            outputBuffer[outputIndex + 3] = p.B;

            LOG(glog, DEBUG_3 + 1, "Row: %d Column %d Offs %d Code 0x%x RGB %d %d %d\n", i, j, params.width * bpp * i + 3 *j + 1, index,  p.R, p.G, p.B);
        }
    }
}


// Private Functions

pngMetaData _process_IHDR(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    return process_PNG_header(chunkBuffer);
}

void _process_IDAT(uint8_t *chunkBuffer, uint32_t chunkLength, uint8_t **outputBuffer, uint32_t *outputLength)
{
    *outputLength += chunkLength;
	*outputBuffer = realloc(*outputBuffer, *outputLength * sizeof(uint8_t));
	memcpy(*outputBuffer + (*outputLength - chunkLength), chunkBuffer, chunkLength);
}

void  _process_PLTE(uint8_t *chunkBuffer, uint32_t chunkLength, pallete *plte)
{

    for(int i = 0; i < 256; i++)
    {
        plte->buffer[i].R = chunkBuffer[3 * i];
        plte->buffer[i].G = chunkBuffer[3 * i + 1];
        plte->buffer[i].B = chunkBuffer[3 * i + 2];

        LOG(glog, DEBUG, "Pallete entry: 0x%x R: %d G: %d B: %d\n", i, plte->buffer[i].R,
        plte->buffer[i].G, plte->buffer[i].B);
    }

}

// Ancillary chunks

void _process_tEXt(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    int keyLength = strlen(chunkBuffer);
    LOG(glog, INFO, "%s: %.*s\n", chunkBuffer, chunkLength - keyLength - 1, chunkBuffer + keyLength + 1);
}
void _process_sRGB(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "sRGB chunks not yet implemented\n");
}
void _process_iTXt(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "iTXt chunkks not yet complete\n");
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
    LOG(glog, INFO, "%s: ", chunkBuffer);
    
    // There may be null characters in the string
    for(int i = offset; i < chunkLength; i++)
    {
        printf("%c", chunkBuffer[i]);
    }
    printf("\n");
}
void _process_zTXt(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Not yet complete\n");

    // Not sure if deflate or zlib
    uint32_t offset = 0;
    int keyLength = strlen(chunkBuffer);
    offset += keyLength + 1;
    
    // Compression method but currently only deflate
    uint8_t compressionMethod = *(chunkBuffer + offset++); 
    uint8_t *outputMessage = malloc(1024 *sizeof(uint8_t)); 
    bitstream_t message;

    create_bitstream(&message, chunkBuffer + offset, chunkLength - offset); 
    inflate(&message, outputMessage);

    LOG(glog, INFO, "%s: %s\n", chunkBuffer, outputMessage);

    delete_bitstream(&message);
    free(outputMessage);
}

void _process_bKGD(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Background chunk not yet implemented\n");
}

void _process_pHYs(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Physical dimensions chunk not yet implemented\n");
}

void _process_sBIT(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Significant bits chunk not yet implemented\n");
}

void _process_sPLT(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Suggested pallete chunk not yet implemented\n");
}

void _process_hIST(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Histogram chunk not yet implemented\n");
}

void _process_tIME(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Last modified time chunk not yet implemented\n");
}

void _process_tRNS(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Transparency chunk not yet implemented\n");
}

void _process_gAMA(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Gamma chunk not yet implemented\n");
}

void _process_cHRM(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "Primary chromaticity chunk not yet implemented\n");
}

void _process_iCCP(uint8_t *chunkBuffer, uint32_t chunkLength)
{
    LOG(glog, WARNING, "ICC profile chunk not yet implemented\n");
}

