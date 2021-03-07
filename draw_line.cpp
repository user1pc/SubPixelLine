
#include "draw_line.h"
#include "line_traverser.h"

typedef struct 
{
    uint32_t *pixels;
    uint32_t color;
    int width, height;
} DrawLineImageInfo;

void draw_line_pixel_setter(int32_t x, int32_t y, void *user_data)
{
    DrawLineImageInfo *p_info = (DrawLineImageInfo*)user_data;
    if (x >= 0 && x < p_info->width && y >= 0 && y < p_info->height)
        p_info->pixels[y * p_info->width + x] = p_info->color;
}

void drawline_include_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, uint32_t color,
    uint32_t *pixels, int width, int height)
{
    DrawLineImageInfo info;
    info.pixels = pixels;
    info.width = width;
    info.height = height;
    info.color = color;
    LineTraverser_traverse_include_endpoints(x1, y1, x2, y2, pixel_width, draw_line_pixel_setter, &info);
}

void drawline_exclude_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, uint32_t color,
    uint32_t *pixels, int width, int height)
{
    DrawLineImageInfo info;
    info.pixels = pixels;
    info.width = width;
    info.height = height;
    info.color = color;
    LineTraverser_traverse_exclude_endpoints(x1, y1, x2, y2, pixel_width, draw_line_pixel_setter, &info);
}