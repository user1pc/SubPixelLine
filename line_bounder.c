
/// line_bounder.c
/// Provides functions for bounding a line inside a rectangle.

#include <math.h>
#include "line_bounder.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

bool line_bounded_fract_less(int32_t a_numerator, int32_t a_denominator, 
    int32_t b_numerator, int32_t b_denominator)
{
    return ((int64_t)a_numerator * b_denominator) < ((int64_t)b_numerator * a_denominator);
}
bool line_bounded_fract_greater(int32_t a_numerator, int32_t a_denominator, 
    int32_t b_numerator, int32_t b_denominator)
{
    return ((int64_t)a_numerator * b_denominator) > ((int64_t)b_numerator * a_denominator);
}

void line_rectangle_intersection_near(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max,
    int32_t *out_near_numerator, int32_t *out_near_denominator)
{
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    int32_t t1x_numerator = ((dx >= 0) ? x_min : x_max) - x1;
    int32_t t1x_denominator = dx;
    int32_t t1y_numerator = ((dy >= 0) ? y_min : y_max) - y1;
    int32_t t1y_denominator = dy;

    int32_t t2x_numerator = ((dx >= 0) ? x_max : x_min) - x1;
    int32_t t2x_denominator = dx;
    int32_t t2y_numerator = ((dy >= 0) ? y_max : y_min) - y1;
    int32_t t2y_denominator = dy;

    t1x_numerator = (t1x_denominator < 0) ? (-t1x_numerator) : t1x_numerator;
    t1x_denominator = abs(t1x_denominator);
    t1y_numerator = (t1y_denominator < 0) ? (-t1y_numerator) : t1y_numerator;
    t1y_denominator = abs(t1y_denominator);

    t2x_numerator = (t2x_denominator < 0) ? (-t2x_numerator) : t2x_numerator;
    t2x_denominator = abs(t2x_denominator);
    t2y_numerator = (t2y_denominator < 0) ? (-t2y_numerator) : t2y_numerator;
    t2y_denominator = abs(t2y_denominator);

    int32_t t1_numerator = t1x_numerator;
    int32_t t1_denominator = t1x_denominator;
    if (line_bounded_fract_greater(t1y_numerator, t1y_denominator, t1_numerator, t1_denominator))
    {
        t1_numerator = t1y_numerator;
        t1_denominator = t1y_denominator;
    }
    *out_near_numerator = t1_numerator;
    *out_near_denominator = t1_denominator;
}

void line_rectangle_intersection_far(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max,
    int32_t *out_far_numerator, int32_t *out_far_denominator)
{
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    int32_t t1x_numerator = ((dx >= 0) ? x_min : x_max) - x1;
    int32_t t1x_denominator = dx;
    int32_t t1y_numerator = ((dy >= 0) ? y_min : y_max) - y1;
    int32_t t1y_denominator = dy;

    int32_t t2x_numerator = ((dx >= 0) ? x_max : x_min) - x1;
    int32_t t2x_denominator = dx;
    int32_t t2y_numerator = ((dy >= 0) ? y_max : y_min) - y1;
    int32_t t2y_denominator = dy;

    t1x_numerator = (t1x_denominator < 0) ? (-t1x_numerator) : t1x_numerator;
    t1x_denominator = abs(t1x_denominator);
    t1y_numerator = (t1y_denominator < 0) ? (-t1y_numerator) : t1y_numerator;
    t1y_denominator = abs(t1y_denominator);

    t2x_numerator = (t2x_denominator < 0) ? (-t2x_numerator) : t2x_numerator;
    t2x_denominator = abs(t2x_denominator);
    t2y_numerator = (t2y_denominator < 0) ? (-t2y_numerator) : t2y_numerator;
    t2y_denominator = abs(t2y_denominator);

    int32_t t2_numerator = t2x_numerator;
    int32_t t2_denominator = t2x_denominator;
    if (line_bounded_fract_less(t2y_numerator, t2y_denominator, t2_numerator, t2_denominator))
    {
        t2_numerator = t2y_numerator;
        t2_denominator = t2y_denominator;
    }
    *out_far_numerator = t2_numerator;
    *out_far_denominator = t2_denominator;
}

bool line_bound_inside_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t bounds_min_x, int32_t bounds_min_y, int32_t bounds_max_x, int32_t bounds_max_y,
    int32_t *out_x1, int32_t *out_y1, int32_t *out_x2, int32_t *out_y2)
{
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    if (dx == 0)
    {
        *out_x1 = x1;
        *out_x2 = x2;
        *out_y1 = max(min(y1, bounds_max_y), bounds_min_y);
        *out_y2 = max(min(y2, bounds_max_y), bounds_min_y);
        bool x_in_bounds = x1 >= bounds_min_x && x1 <= bounds_max_x;
        bool y_in_bounds = min(y1, y2) <= bounds_max_y && max(y1, y2) >= bounds_min_y;
        return x_in_bounds && y_in_bounds && *out_y2 != *out_y1;
    }
    if (dy == 0)
    {
        *out_y1 = y1;
        *out_y2 = y2;
        *out_x1 = max(min(x1, bounds_max_x), bounds_min_x);
        *out_x2 = max(min(x2, bounds_max_x), bounds_min_x);
        bool y_in_bounds = y1 >= bounds_min_y && y1 <= bounds_max_y;
        bool x_in_bounds = min(x1, x2) <= bounds_max_x && max(x1, x2) >= bounds_min_x;
        return x_in_bounds && y_in_bounds && *out_x2 != *out_x1;
    }
    int32_t t_near_numerator, t_near_denominator;
    int32_t t_far_numerator, t_far_denominator;
    line_rectangle_intersection_near(x1, y1, x2, y2, bounds_min_x, bounds_max_x, bounds_min_y, bounds_max_y,
        &t_near_numerator, &t_near_denominator);
    line_rectangle_intersection_far(x1, y1, x2, y2, bounds_min_x, bounds_max_x, bounds_min_y, bounds_max_y,
        &t_far_numerator, &t_far_denominator);
    
    *out_x1 = x1;
    *out_y1 = y1;
    *out_x2 = x2;
    *out_y2 = y2;
    if ((t_near_numerator ^ t_near_denominator) > 0)
    {
        if (t_near_denominator != 0)
        {
            *out_x1 = (x1 * t_near_denominator + t_near_numerator * dx)  / t_near_denominator;
            *out_y1 = (y1 * t_near_denominator + t_near_numerator * dy)  / t_near_denominator;
        }
    }
    if (t_far_numerator < t_far_denominator)
    {
        if (t_far_denominator != 0)
        {
            *out_x2 = (x1 * t_far_denominator + t_far_numerator * dx)  / t_far_denominator;
            *out_y2 = (y1 * t_far_denominator + t_far_numerator * dy)  / t_far_denominator;
        }
    }
    bool hit = line_bounded_fract_less(t_near_numerator, t_near_denominator, t_far_numerator, t_far_denominator);
    hit &= t_near_numerator < t_near_denominator;
    hit &= t_far_numerator > 0;
    return hit;
}
