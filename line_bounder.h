
/// line_bounder.h
/// Provides functions for bounding a line inside a rectangle.

#ifndef LINE_BOUNDER_H
#define LINE_BOUNDER_H

#include "line_traverser.h"

/// Bounds a line to a rectangle region
/// @param x1 is the 1st x point of the line.
/// @param y1 is the 1st y point of the line.
/// @param x2 is the 2nd x point of the line.
/// @param y2 is the 2nd y point of the line.
/// @param box_min_x is the inclusive minimum x value to bound the line within.
/// @param box_min_y is the inclusive minimum y value to bound the line within.
/// @param box_min_y is the inclusive maximum x value to bound the line within.
/// @param box_max_y is the inclusive maximum y value to bound the line within.
/// @param out_x1 is a pointer to the new x1 coordinate after bounding.
/// @param out_y1 is a pointer to the new y1 coordinate after bounding.
/// @param out_x2 is a pointer to the new x2 coordinate after bounding.
/// @param out_y2 is a pointer to the new y2 coordinate after bounding.
/// @returns true if the line segment returned is inside the region, false otherwise.
bool line_bound_inside_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t bounds_min_x, int32_t bounds_min_y, int32_t bounds_max_x, int32_t bounds_max_y,
    int32_t *out_x1, int32_t *out_y1, int32_t *out_x2, int32_t *out_y2);

#endif // LINE_BOUNDER_H
