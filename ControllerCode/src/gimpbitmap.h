#ifndef _GIMPBITMAP_
#define _GIMPBITMAP_

// this structre can hold up to a 32x32 pixel RGB bitmap
struct bitmap {
  unsigned int   width;
  unsigned int   height;
  unsigned int   bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char  pixel_data[38*28*3 + 1];
};

#endif