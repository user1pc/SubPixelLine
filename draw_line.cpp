
#include "draw_line.h"
#include <math.h>

void drawline_exclude_endpoints(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
uint32_t *image_pixels, int image_width, int image_height)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x_step = (dx >= 0) ? 1 : -1;
    int y_step = (dy >= 0) ? 1 : -1;
    int next_pixel_x = (dx >= 0) ? pixel_width : -1;
    int next_pixel_y = (dy >= 0) ? pixel_width : -1;
    int local_x = x1 % pixel_width;
    int local_y = y1 % pixel_width;
    int clockwiseness = dx * (next_pixel_y - local_y) - dy * (next_pixel_x - local_x);
    int dx_clockwiseness = -dy * pixel_width;
    int dy_clockwiseness = dx * pixel_width;

    int x = x1 / pixel_width;
    int y = y1 / pixel_width;
    int end_x = x2 / pixel_width;
    int end_y = y2 / pixel_width;
    while (true)
    {
        int next_clockwiseness = clockwiseness;
        if (clockwiseness >= 0)
        {
            x += x_step;
            next_clockwiseness += dx_clockwiseness;
        }
        if (clockwiseness <= 0)
        {
            y += y_step;
            next_clockwiseness += dy_clockwiseness;
        }
        clockwiseness = next_clockwiseness;

        // End condition comes first, because we don't want to draw
        // the endpoints of the line.
        if (x == end_x && y == end_y)
            break;

        // Set the pixel AFTER incrimenting the point on the line,
        // because this function is NOT supposed to draw the endpoints of the line.
        if (y >= 0 && y < image_height && x >= 0 && x < image_width)
            image_pixels[y * image_width + x] = color;
    }
}

void drawline_include_endpoints(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
uint32_t *image_pixels, int image_width, int image_height)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x_step = (dx >= 0) ? 1 : -1;
    int y_step = (dy >= 0) ? 1 : -1;
    int next_pixel_x = (dx >= 0) ? pixel_width : -1;
    int next_pixel_y = (dy >= 0) ? pixel_width : -1;
    int local_x = x1 % pixel_width;
    int local_y = y1 % pixel_width;
    int clockwiseness = dx * (next_pixel_y - local_y) - dy * (next_pixel_x - local_x);
    int dx_clockwiseness = -dy * pixel_width;
    int dy_clockwiseness = dx * pixel_width;

    int x = x1 / pixel_width;
    int y = y1 / pixel_width;
    int end_x = x2 / pixel_width;
    int end_y = y2 / pixel_width;
    if (x2 % pixel_width && y2 % pixel_width)
    {
        // The endpoint closest to (+INF, +INF) must be rounded up 1 if the endpoint lies
        // inside a pixel, effectively ceiling it.
        local_x = x2 % pixel_width;
        local_y = y2 % pixel_width;
        int end_clockwiseness = dx * (next_pixel_y - local_y) - dy * (next_pixel_x - local_x);
        if (end_clockwiseness >= 0)
            end_x++;
        if (end_clockwiseness <= 0)
            end_y++;
    }
    while (true)
    {
        if (x == end_x && y == end_y)
            break;

        if (y >= 0 && y < image_height && x >= 0 && x < image_width)
            image_pixels[y * image_width + x] = color;

        int next_clockwiseness = clockwiseness;
        if (clockwiseness >= 0)
        {
            x += x_step;
            next_clockwiseness += dx_clockwiseness;
        }
        if (clockwiseness <= 0)
        {
            y += y_step;
            next_clockwiseness += dy_clockwiseness;
        }
        clockwiseness = next_clockwiseness;
    }
}
