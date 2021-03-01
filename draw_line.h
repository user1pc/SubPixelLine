
#ifndef _DRAW_LINE_H
#define _DRAW_LINE_H

#include <stdint.h>

void drawline(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
uint32_t *image_pixels, int image_width, int image_height);

#endif // _DRAW_LINE_H
