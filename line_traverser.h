
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

LineTraverser LineTraverser_init(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width);
bool LineTraverser_is_end(const LineTraverser *p_traverser);
void LineTraverser_get_point(const LineTraverser *p_traverser, int32_t *out_x, int32_t *out_y);
void LineTraverser_next(LineTraverser *p_traverser);

typedef void (*LineTraverserCallback)(int32_t x, int32_t y, void *user_data);

void LineTraverser_traverse_include_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, 
    LineTraverserCallback callback, void *user_data);

void LineTraverser_traverse_exclude_endpoints(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t pixel_width, 
    LineTraverserCallback callback, void *user_data);

#endif // LINE_TRAVERSER_H
