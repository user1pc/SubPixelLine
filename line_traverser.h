
/// line_traverser.h
/// Provides functions for traversing a line over a grid, where the starting and ending points
/// have sub-grid coordinates.

#ifndef LINE_TRAVERSER_H
#define LINE_TRAVERSER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int64_t clockwiseness;
    int64_t dx_clockwiseness;
    int64_t dy_clockwiseness;
    int32_t x, y;
    int32_t dx_x, dy_y;
    int32_t end_x, end_y;
} LineTraverser;

/// Initializes a LineTraverser for a given line.
/// @param x1 is the x coordinate of the starting point of the line.
/// @param y1 is the y coordinate of the starting point of the line.
/// @param x2 is the x coordinate of the ending point of the line.
/// @param y2 is the y coordinate of the ending point of the line.
/// @param square_width is the width of a square in the grid being traversed.
/// @returns a LineTraverser which can be used to traverse all grid points which intersect the line.
/// @remarks This will traverse every grid point which intersects the line. If the line intersects only
/// a corner of a grid square, that grid square will not be traversed. If the line intersects the bottom (min-y)
/// side of a grid square and is perfectly horizontal, that grid square will be traversed.
/// If the line intersects the left (min-x) side of a grid square and is perfectly vertical,
/// that grid square will be traversed.
LineTraverser LineTraverser_init(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width);

/// Tests if the current grid square in the traverser is the endpoint of the line.
/// @param p_traverser is a pointer to the traverser to test.
/// @returns true if this current grid square is the last one. False otherwise.
/// @remarks The last grid square corresponds to the point (x2,y2) in the call to LineTraverser_init().
/// This will only return true ON the last grid square. If LineTraverser_next() is called
/// past the end of the line, this function will never return true again (until integer overflow).
/// So you must stop the traversing loop once this returns true, or define your own loop exit condition.
bool LineTraverser_is_end(const LineTraverser *p_traverser);

/// Gets the current grid square coordinate of a LineTraverser.
/// @param p_traverser is a pointer to the traverser to get the coordinate of.
/// @param out_x is a pointer to write the x coordinate to.
/// @param out_y is a pointer to write the y coordinate to.
void LineTraverser_get_point(const LineTraverser *p_traverser, int32_t *out_x, int32_t *out_y);

/// Updates a LineTraverser to the next grid coordinate.
/// @param p_traverser is a pointer to the traverser to update.
void LineTraverser_next(LineTraverser *p_traverser);

/// Gets the grid-coordinates of the endpoints of a line.
/// @param x1 is the x coordinate of the starting point of the line.
/// @param y1 is the y coordinate of the starting point of the line.
/// @param x2 is the x coordinate of the ending point of the line.
/// @param y2 is the y coordinate of the ending point of the line.
/// @param square_width is the width of a square in the grid being traversed.
/// @param include_endpoints is set to true if you want to consider the endpoints of the line,
/// or false to exclude the grid squares on the endpoints.
/// @param out_x1 is a pointer to write the x coordinate which corresponds to x1 in grid-space coordinates.
/// @param out_y1 is a pointer to write the y coordinate which corresponds to y1 in grid-space coordinates.
/// @param out_x2 is a pointer to write the x coordinate which corresponds to x2 in grid-space coordinates.
/// @param out_y2 is a pointer to write the y coordinate which corresponds to y2 in grid-space coordinates.
/// @remarks It may seem intuitive that the endpoint grid-coordinates can be obtained by dividing the line
/// endpoints by square_width, however this is not true. This function will get the correct grid coordinates
/// that corresponds to the line endpoints.
/// Usually, you will probably want to set include_endpoints to true. But it can be set to false in case you
/// don't want to include the endpoints. To exclude the endpoints means to skip exactly one square towards
/// both ends of the line. So, setting include_endpoints to false will exclude exactly one grid square
/// on each side of the line.
void LineTraverser_get_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width, bool include_endpoints,
    int32_t *out_x1, int32_t *out_y1, int32_t *out_x2, int32_t *out_y2);

/// User defined function which can be called during a line traversal.
/// @param x is the x coordinate of current grid square that intersects the line.
/// @param y is the y coordinate of current grid square that intersects the line.
/// @param user_data is a pointer to user defined data.
typedef void (*LineTraverserCallback)(int32_t x, int32_t y, void *user_data);

/// Traverser all grid squares that intersects a line.
/// @param x1 is the x coordinate of the starting point of the line.
/// @param y1 is the y coordinate of the starting point of the line.
/// @param x2 is the x coordinate of the ending point of the line.
/// @param y2 is the y coordinate of the ending point of the line.
/// @param square_width is the width of a square in the grid being traversed.
/// @param callback is a user-defined function which will be called for every point on the line.
/// @param user_data is a pointer to user data passed to callback().
/// @remarks This will traverse every grid point which intersects the line. If the line intersects only
/// a corner of a grid square, that grid square will not be traversed. If the line intersects the bottom (min-y)
/// side of a grid square and is perfectly horizontal, that grid square will be traversed.
/// If the line intersects the left (min-x) side of a grid square and is perfectly vertical,
/// that grid square will be traversed.
/// The meaning of "include_endpoints" is that both starting and ending grid squares will be called during
/// traversal. The alternative is LineTraverser_traverse_exclude_endpoints() which will exclude exactly one
/// endpoint on each side of the line.
void LineTraverser_traverse_include_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width, 
    LineTraverserCallback callback, void *user_data);

/// Traverser all grid squares that intersects a line, excluding the starting and ending points.
/// @param x1 is the x coordinate of the starting point of the line.
/// @param y1 is the y coordinate of the starting point of the line.
/// @param x2 is the x coordinate of the ending point of the line.
/// @param y2 is the y coordinate of the ending point of the line.
/// @param square_width is the width of a square in the grid being traversed.
/// @param callback is a user-defined function which will be called for every point on the line.
/// @param user_data is a pointer to user data passed to callback().
/// @remarks This will traverse every grid point which intersects the line (excluding endpoints).
/// If the line intersects only a corner of a grid square, that grid square will not be traversed.
/// If the line intersects the bottom (min-y) side of a grid square and is perfectly horizontal,
/// that grid square will be traversed. If the line intersects the left (min-x) side of a grid square
///  and is perfectly vertical, that grid square will be traversed.
/// The meaning of "exclude_endpoints" is that both starting and ending grid squares will be excluded during
/// traversal. They will be excluded, even if the endpoint lies exactly on the edge of a grid square.
/// The alternative is LineTraverser_traverse_include_endpoints() which will call callback() for every point
/// that intersects the line. Usually, you probably want to use LineTraverser_traverse_include_endpoints().
void LineTraverser_traverse_exclude_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t square_width, 
    LineTraverserCallback callback, void *user_data);

#endif // LINE_TRAVERSER_H
