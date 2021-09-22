#include "image.h"


void defilter(uint8_t *outputBuffer, uint8_t *buffer, uint32_t height, uint32_t scanwidth, uint8_t bpp)
{

    int lines = height;
    int width = scanwidth - 1;
    int offset = 0;
    int outputOffset = 0;
    
    uint8_t *prevScanline = malloc(width);
    uint8_t *scanline = malloc(width);
        
    memset(scanline, 0, width);
    memset(prevScanline, 0, width);
    
    for(int i = 0; i < lines; i++)
    {
        LOG(glog, DEBUG_2, "line %d offset %d\n", i, offset);

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
    if(buffer != NULL)
    {
        return *buffer;
    }
    else
    {
        LOG(glog, ERROR, "Scanline buffer is null\n");
        return -1;
    }
}

void read_scanline(uint8_t *buffer, uint32_t length, uint8_t *scanline)
{
    for(int i = 0; i < length; i++)
    {
        scanline[i] = buffer[i];
    }
}

void defilter_adam7(uint8_t *outputBuffer, uint8_t *buffer, pngMetaData params, int pass)
{
    if(pass > 6)
    {
        LOG(glog, ERROR, "Only 7 passes allowed\n");
        return; 
    }

    int bpp = get_bytes_per_pixel(params);
    int offset = 0;
    int outputOffset = 0;
    int outputSize = params.width * params.height * bpp;
    
    struct pass_data
    {
        int x_start;
        int x_offset;
        int y_start;
        int y_offset;
    };
  
    const struct pass_data adam7[7] = {
    [0] = {
        .x_start = 0,
        .x_offset = 8,
        .y_start = 0,
        .y_offset = 8,
    },
    [1] = {
        .x_start = 4,
        .x_offset = 8,
        .y_start = 0,
        .y_offset = 8,
    },
    [2] = 
    {
        .x_start = 0,
        .x_offset = 4,
        .y_start = 4,
        .y_offset = 8,
    },
    [3] = 
    {
        .x_start = 2,
        .x_offset = 4,
        .y_start = 0,
        .y_offset = 4,
    },
    [4] = 
    {
        .x_start = 0,
        .x_offset = 2,
        .y_start = 2,
        .y_offset = 4,
    },
    [5] = 
    {
        .x_start = 1,
        .x_offset = 2,
        .y_start = 0,
        .y_offset = 2,
    },
    [6] = 
    {
        .x_start = 0,
        .x_offset = 1,
        .y_start = 1,
        .y_offset = 2,
     },
    };

    int scan_width =  1 + ((params.width * bpp) / adam7[pass].x_offset);
    int scan_lines  = params.height / adam7[pass].y_offset; 
    int pass_size  = scan_lines * scan_width;

    LOG(glog, DEBUG, "scan width: %d scan lines: %d total size: %d\n", scan_width, scan_lines,
    pass_size);

    int pixel_width = (scan_width - 1) / bpp;
    int width = scan_width - 1;
    int height = scan_lines;

    LOG(glog, DEBUG, "Width: %d Height: %d\n", width, height);

    uint8_t *pass_buffer = malloc(pass_size);
    defilter(pass_buffer, buffer, scan_lines, scan_width, bpp);  
   
    // assemble adam7 image
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j+=bpp)
        {
            // this is the output image use the actual width not the reduced one
            outputOffset = params.width * bpp * (adam7[pass].y_start + adam7[pass].y_offset * i) +
            (bpp * adam7[pass].x_start + adam7[pass].x_offset * j);
            LOG(glog, DEBUG_3 + 1, "offset: %d\n", outputOffset); 
            outputBuffer[outputOffset] =  pass_buffer[i * width +  j];
            outputBuffer[outputOffset + 1] =  pass_buffer[i * width +  j +1];
            outputBuffer[outputOffset + 2] =  pass_buffer[i * width +  j + 2];
        }
    }

    free(pass_buffer);
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
