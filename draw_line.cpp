
#include "draw_line.h"
#include <math.h>

void drawline_exclude_endpoints(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
uint32_t *image_pixels, int image_width, int image_height)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x_step = (dx >= 0) ? 1 : -1;
    int y_step = (dy >= 0) ? 1 : -1;
    int next_pixel_x = (dx >= 0) ? pixel_width : 0;
    int next_pixel_y = (dy >= 0) ? pixel_width : 0;
    int local_x = x1 % pixel_width;
    int local_y = y1 % pixel_width;
    int x_dist = (dx >= 0) ? (pixel_width - local_x) : (local_x);
    int y_dist = (dy >= 0) ? (pixel_width - local_y) : (local_y);
    int clockwiseness = abs(dx) * abs(y_dist) - abs(dy) * abs(x_dist);
    int dx_clockwiseness = -abs(dy) * pixel_width;
    int dy_clockwiseness = abs(dx) * pixel_width;

    int x = x1 / pixel_width;
    int y = y1 / pixel_width;
    int end_x = x2 / pixel_width;
    int end_y = y2 / pixel_width;
    if (dy < 0)
    {
        if ((y1 % pixel_width) == 0)
        {
            y--;
            clockwiseness += dy_clockwiseness;
        }
    }
    else if (dy > 0)
    {
        if ((y2 % pixel_width) == 0)
            end_y--;
    }

    if (dx < 0)
    {
        if ((x1 % pixel_width) == 0)
        {
            x--;
            clockwiseness += dx_clockwiseness;
        }
    }
    else if (dx > 0)
    {
        if ((x2 % pixel_width) == 0)
            end_x--;
    }
    if (x == end_x && y == end_y)
        return;
    while (true)
    {
        int old_clockwiseness = clockwiseness;
        if (old_clockwiseness >= 0)
        {
            x += x_step;
            clockwiseness += dx_clockwiseness;
        }
        if (old_clockwiseness <= 0)
        {
            y += y_step;
            clockwiseness += dy_clockwiseness;
        }

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
    int next_pixel_x = (dx >= 0) ? pixel_width : 0;
    int next_pixel_y = (dy >= 0) ? pixel_width : 0;
    int local_x = x1 % pixel_width;
    int local_y = y1 % pixel_width;
    int x_dist = (dx >= 0) ? (pixel_width - local_x) : (local_x);
    int y_dist = (dy >= 0) ? (pixel_width - local_y) : (local_y);
    int clockwiseness = abs(dx) * abs(y_dist) - abs(dy) * abs(x_dist);
    int dx_clockwiseness = -abs(dy) * pixel_width;
    int dy_clockwiseness = abs(dx) * pixel_width;

    int x = x1 / pixel_width;
    int y = y1 / pixel_width;
    int end_x = x2 / pixel_width;
    int end_y = y2 / pixel_width;
    if (dy < 0)
    {
        if ((y1 % pixel_width) == 0)
        {
            y--;
            clockwiseness += dy_clockwiseness;
        }
    }
    else if (dy > 0)
    {
        if ((y2 % pixel_width) == 0)
            end_y--;
    }

    if (dx < 0)
    {
        if ((x1 % pixel_width) == 0)
        {
            x--;
            clockwiseness += dx_clockwiseness;
        }
    }
    else if (dx > 0)
    {
        if ((x2 % pixel_width) == 0)
            end_x--;
    }
    while (true)
    {
        if (y >= 0 && y < image_height && x >= 0 && x < image_width)
            image_pixels[y * image_width + x] = color;

        if (x == end_x && y == end_y)
            break;

        int old_clockwiseness = clockwiseness;
        if (old_clockwiseness >= 0)
        {
            x += x_step;
            clockwiseness += dx_clockwiseness;
        }
        if (old_clockwiseness <= 0)
        {
            y += y_step;
            clockwiseness += dy_clockwiseness;
        }
    }
}
