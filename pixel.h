#ifndef PIXEL_H
#define PIXEL_H

// Store one pixel's color and position.
struct _pixel
{
    float r;
    float g;
    float b;
    int x;
    int y;
};

// Use Pixel as the shorter type name.
typedef struct _pixel Pixel;

#endif
