
#include "draw_line.h"

void drawline(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
uint32_t *image_pixels, int image_width, int image_height)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
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
        // Set the pixel
        if (y >= 0 && y < image_height && x >= 0 && x < image_width)
            image_pixels[y * image_width + x] = color;

        // End condition
        if (x == end_x && y == end_y)
            break;

        int next_clockwiseness = clockwiseness;
        if (clockwiseness >= 0)
        {
            x++;
            next_clockwiseness += dx_clockwiseness;
        }
        if (clockwiseness <= 0)
        {
            y++;
            next_clockwiseness += dy_clockwiseness;
        }
        clockwiseness = next_clockwiseness;
    }
}
