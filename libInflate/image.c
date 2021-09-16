#include "image.h"


void defilter(uint8_t *outputBuffer, uint8_t *buffer, pngMetaData params)
{

    int lines = params.height;
    int bpp = params.bit_depth / 8;
	
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

    int scanwidth = params.width * bpp + 1;
    int width = scanwidth - 1;
    int offset = 0;
    int outputOffset = 0;
    
    LOG(glog, DEBUG, "BPP: %d\n", bpp);    
   
    uint8_t *prevScanline = malloc(width);
    uint8_t *scanline = malloc(width);
        
    memset(scanline, 0, width);
    memset(prevScanline, 0, width);
    
    for(int i = 0; i < lines; i++)
    {
        uint8_t filter_type = read_filter_byte(buffer + offset++);
        read_scanline(buffer + offset, width, scanline);
        offset += width;

        switch(filter_type)
        {
            case FILTER_TYPE_NONE:
                LOG(glog, DEBUG_1, "Filter type NONE\n");
                filter_none(outputBuffer + outputOffset, width, scanline);
            break;
            case FILTER_TYPE_SUB:
                LOG(glog, DEBUG_1, "Filter type SUB\n");
                filter_sub(outputBuffer + outputOffset, width, scanline, bpp);
            break;
            case FILTER_TYPE_UP:
                LOG(glog, DEBUG_1, "Filter type UP\n");                
                filter_up(outputBuffer + outputOffset, width, scanline, prevScanline);
            break;
            case FILTER_TYPE_AVERAGE:
                LOG(glog, DEBUG_1, "Filter type AVERAGE\n");
                filter_average(outputBuffer + outputOffset, width, scanline, prevScanline, bpp);
            break;
            case FILTER_TYPE_PAETH:
                LOG(glog, DEBUG_1, "Filter type PAETH\n");
                filter_paeth(outputBuffer + outputOffset, width, scanline, prevScanline, bpp);
            break;
            default:
                LOG(glog, ERROR, "Invalid filter type: %d\n", filter_type);
            break;
        }
        outputOffset += width;
        memcpy(prevScanline, scanline, width);
    }
    
    free(scanline);
    free(prevScanline);
}

uint8_t read_filter_byte(uint8_t *buffer)
{
    return *buffer;
}

void read_scanline(uint8_t *buffer, uint32_t length, uint8_t *scanline)
{
    for(int i = 0; i < length; i++)
    {
        scanline[i] = buffer[i];
    }
}

void filter_none(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline)
{

    for(int i = 0; i < length; i++)
    {
        outputBuffer[i] = scanline[i];
    }
}

void filter_sub(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t bpp)
{
    uint8_t prev = 0;
    uint32_t reconstructed = 0;
    
    for(int i = 0; i < length; i++)
    {
        if(i < bpp)
        {
            prev = 0;
        }
        else
        {
            prev = scanline[i - bpp];
        }
    
        reconstructed = scanline[i] + prev;
        scanline[i] = reconstructed & 0xFF;
        outputBuffer[i] = scanline[i];
    }

}

void filter_up(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline)
{
    uint8_t up = 0;

    for(int i = 0; i < length; i++)
    {
        up  = prevScanline[i];
        scanline[i] += up;
        outputBuffer[i] = scanline[i];
    }

}

void filter_average(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t
*prevScanline, uint8_t bpp)
{
    
    uint16_t prev = 0, up = 0;

    for(int i = 0; i < length; i++)
    {
        if(i < bpp)
        {
            prev = 0;
        }
        else
        {
            prev = scanline[i - bpp];
        }

        up = prevScanline[i];
        scanline[i] = scanline[i] + ((prev + up) >> 1);
        outputBuffer[i] = scanline[i];
    }

}

void filter_paeth(uint8_t *outputBuffer, uint32_t length, uint8_t *scanline, uint8_t *prevScanline, uint8_t bpp)
{
    for(int i = 0; i < length; i++)
    {
        uint8_t paeth_left, paeth_top_left, paeth_top; 
        
        if(i < bpp)
        {
            paeth_left = 0;
            paeth_top_left = 0;
        }
        else
        {
            paeth_top_left = prevScanline[i - bpp];
            paeth_left = scanline[i - bpp];
        }

        paeth_top = prevScanline[i];

        int res = paeth_left + paeth_top - paeth_top_left; 
        int chosen = 0;
        int reconstructed = 0;

        int res_left = abs(res - paeth_left);
        int res_top  = abs(res - paeth_top);
        int res_top_left = abs(res - paeth_top_left);

        if((res_left <= res_top) && (res_left <= res_top_left))
        {
            chosen = paeth_left;
        }
        else if(res_top <= res_top_left)
        {
            chosen = paeth_top;
        }
        else
        {
            chosen = paeth_top_left;
        }

        reconstructed = scanline[i] + chosen;
        scanline[i] = reconstructed & 0xFF;
        outputBuffer[i] = scanline[i];
    }   
}
