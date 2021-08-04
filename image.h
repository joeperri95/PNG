#ifndef __IMAGE_H__
#define __IMAGE_H__

// Take output of zlib stream and process into final image

void read_filter_byte();
void read_scanline();
void read_scanline_adam7();

void filter_none();
void filter_sub();
void filter_up();
void filter_average();
void filter_paeth();

#endif
