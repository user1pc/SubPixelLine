
/// line_traverser.cpp
/// Provides functions for traversing a line over a grid, where the starting and ending points
/// have sub-grid coordinates.

#include "line_traverser.h"
#include <math.h>

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

LineTraverser LineTraverser_init(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width)
{
    LineTraverser traverser;
    int dx = x2 - x1;
    int dy = y2 - y1;
    traverser.dx_x = (dx >= 0) ? 1 : -1;
    traverser.dy_y = (dy >= 0) ? 1 : -1;
    int local_x = x1 % square_width;
    int local_y = y1 % square_width;
    int x_dist = (dx >= 0) ? (square_width - local_x) : (local_x);
    int y_dist = (dy >= 0) ? (square_width - local_y) : (local_y);
    traverser.clockwiseness = (int64_t)abs(dx) * abs(y_dist) - (int64_t)abs(dy) * abs(x_dist);
    traverser.dx_clockwiseness = -(int64_t)abs(dy) * square_width;
    traverser.dy_clockwiseness = (int64_t)abs(dx) * square_width;

    traverser.x = x1 / square_width;
    traverser.y = y1 / square_width;
    traverser.end_x = x2 / square_width;
    traverser.end_y = y2 / square_width;
    if (dy < 0)
    {
        if ((y1 % square_width) == 0)
        {
            traverser.y--;
            traverser.clockwiseness += traverser.dy_clockwiseness;
        }
    }
    else if (dy > 0)
    {
        if ((y2 % square_width) == 0)
            traverser.end_y--;
    }

    if (dx < 0)
    {
        if ((x1 % square_width) == 0)
        {
            traverser.x--;
            traverser.clockwiseness += traverser.dx_clockwiseness;
        }
    }
    else if (dx > 0)
    {
        if ((x2 % square_width) == 0)
            traverser.end_x--;
    }
    return traverser;
}

bool LineTraverser_is_end(const LineTraverser *p_traverser)
{
    return p_traverser->x == p_traverser->end_x &&
        p_traverser->y == p_traverser->end_y;
}

void LineTraverser_next(LineTraverser *p_traverser)
{
    int old_clockwiseness = p_traverser->clockwiseness;
    if (old_clockwiseness >= 0)
    {
        p_traverser->x += p_traverser->dx_x;
        p_traverser->clockwiseness += p_traverser->dx_clockwiseness;
    }
    if (old_clockwiseness <= 0)
    {
        p_traverser->y += p_traverser->dy_y;
        p_traverser->clockwiseness += p_traverser->dy_clockwiseness;
    }
}

void LineTraverser_get_point(const LineTraverser *p_traverser, int32_t *out_x, int32_t *out_y)
{
    *out_x = p_traverser->x;
    *out_y = p_traverser->y;
}

void LineTraverser_get_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width,
    int32_t *out_x1, int32_t *out_y1, int32_t *out_x2, int32_t *out_y2)
{
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t endpoint1_x = x1 / square_width;
    int32_t endpoint1_y = y1 / square_width;
    int32_t endpoint2_x = x2 / square_width;
    int32_t endpoint2_y = y2 / square_width;

    if (dy < 0)
    {
        if ((y1 % square_width) == 0)
        {
            endpoint1_y--;
        }
    }
    else if (dy > 0)
    {
        if ((y2 % square_width) == 0)
            endpoint2_y--;
    }

    if (dx < 0)
    {
        if ((x1 % square_width) == 0)
        {
            endpoint1_x--;
        }
    }
    else if (dx > 0)
    {
        if ((x2 % square_width) == 0)
            endpoint2_x--;
    }

    *out_x1 = endpoint1_x;
    *out_y1 = endpoint1_y;
    *out_x2 = endpoint2_x;
    *out_y2 = endpoint2_y;
}


void LineTraverser_traverse_include_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width, 
    LineTraverserCallback callback, void *user_data)
{
    LineTraverser traverser = LineTraverser_init(x1, y1, x2, y2, square_width);
    while (true)
    {
        int32_t x, y;
        LineTraverser_get_point(&traverser, &x, &y);
        callback(x, y, user_data);
        if (LineTraverser_is_end(&traverser))
            break;
        LineTraverser_next(&traverser);
    }
}

void LineTraverser_traverse_exclude_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width, 
    LineTraverserCallback callback, void *user_data)
{
    LineTraverser traverser = LineTraverser_init(x1, y1, x2, y2, square_width);
    if (LineTraverser_is_end(&traverser))
        return;
    while (true)
    {
        LineTraverser_next(&traverser);
        if (LineTraverser_is_end(&traverser))
            break;
        int32_t x, y;
        LineTraverser_get_point(&traverser, &x, &y);
        callback(x, y, user_data);
    }
}
