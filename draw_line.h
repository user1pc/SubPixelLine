
#ifndef DRAW_LINE_H
#define DRAW_LINE_H

#include <stdint.h>

void drawline_include_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, uint32_t color,
    uint32_t *pixels, int width, int height);

void drawline_exclude_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, uint32_t color,
    uint32_t *pixels, int width, int height);

#endif // DRAW_LINE_H
