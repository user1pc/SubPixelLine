
#include <stdlib.h>
#include <utility>
#include <vector>
#include "gtest/gtest.h"
#include "draw_line.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

bool line_intersects_pixel(int x1, int y1, int x2, int y2, int pixel_x, int pixel_y, int pixel_width)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (pixel_x == x1 / pixel_width && pixel_y == y1 / pixel_width)
        return true;

    if (pixel_x == x2 / pixel_width && pixel_y == y2 / pixel_width)
        return true;

    if (dx == 0)
    {
        if (pixel_x * pixel_width <= min(x1, x2) / pixel_width && pixel_x * pixel_width + pixel_width >= max(x1, x2) / pixel_width)
            return true;
        else
            return false;
    }
    if (dy == 0)
    {
        if (pixel_y * pixel_width <= min(y1, y2) / pixel_width && pixel_y * pixel_width + pixel_width >= max(y1, y2) / pixel_width)
            return true;
        else
            return false;
    }

    int t1x_numerator = ((dx >= 0) ? (pixel_x * pixel_width) : (pixel_x * pixel_width + pixel_width - 0)) - x1;
    int t1x_denominator = dx;
    double dbg_t1x = (double)t1x_numerator / (double)t1x_denominator;
    int t1y_numerator = ((dy >= 0) ? (pixel_y * pixel_width) : (pixel_y * pixel_width + pixel_width - 0)) - y1;
    int t1y_denominator = dy;
    double dbg_t1y = (double)t1y_numerator / (double)t1y_denominator;

    int t2x_numerator = ((dx >= 0) ? (pixel_x * pixel_width + pixel_width - 0) : (pixel_x * pixel_width)) - x1;
    int t2x_denominator = dx;
    double dbg_t2x = (double)t2x_numerator / (double)t2x_denominator;
    int t2y_numerator = ((dy >= 0) ? (pixel_y * pixel_width + pixel_width - 0) : (pixel_y * pixel_width)) - y1;
    int t2y_denominator = dy;
    double dbg_t2y = (double)t2y_numerator / (double)t2y_denominator;

    int near_numerator = t1x_numerator;
    int near_denominator = t1x_denominator;
    double near_t = (double)near_numerator / (double)near_denominator;
    if (t1y_numerator * near_denominator > near_numerator * t1y_denominator)
    {
        near_numerator = t1y_numerator;
        near_denominator = t1y_denominator;
        near_t = (double)near_numerator / (double)near_denominator;
    }

    int far_numerator = t2x_numerator;
    int far_denominator = t2x_denominator;
    double far_t = (double)far_numerator / (double)far_denominator;
    if (t2y_numerator * far_denominator < far_numerator * t2y_denominator)
    {
        far_numerator = t2y_numerator;
        far_denominator = t2y_denominator;
        far_t = (double)far_numerator / (double)far_denominator;
    }

    bool dbg_cmp = near_t < far_t;
    double dbg_near_x = (double)x1 + (double)dx * near_t;
    double dbg_near_y = (double)y1 + (double)dy * near_t;
    double dbg_far_x = (double)x1 + (double)dx * far_t;
    double dbg_far_y = (double)y1 + (double)dy * far_t;

    if (near_numerator * far_denominator > far_numerator * near_denominator)
        return false;
    else if (near_numerator * far_denominator == far_numerator * near_denominator)
        return false;
    else
        return true;
}

void get_starting_pixel(int x1, int y1, int x2, int y2, int pixel_width,
    int *out_x, int *out_y)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx >= 0 && dy >= 0)
    {
        *out_x = x1 / pixel_width;
        *out_y = y1 / pixel_width;
    }
    else if (dx >= 0 && dy < 0)
    {
        if (y1 % pixel_width)
        {
            *out_x = x1 / pixel_width;
            *out_y = y1 / pixel_width;
        }
        else
        {
            *out_x = x1 / pixel_width;
            *out_y = (y1 / pixel_width) - 1;
        }
    }
    else if (dx < 0 && dy >= 0)
    {
        if (x1 % pixel_width)
        {
            *out_x = x1 / pixel_width;
            *out_y = y1 / pixel_width;
        }
        else
        {
            *out_x = (x1 / pixel_width) - 1;
            *out_y = y1 / pixel_width;
        }
    }
    else
    {
        if (x1 % pixel_width)
            *out_x = x1 / pixel_width;
        else
            *out_x = (x1 / pixel_width) - 1;

        if (y1 % pixel_width)
            *out_y = y1 / pixel_width;
        else
            *out_y = (y1 / pixel_width) - 1;
    }
}


void get_ending_pixel(int x1, int y1, int x2, int y2, int pixel_width,
    int *out_x, int *out_y)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx >= 0 && dy >= 0)
    {
        if (x2 % pixel_width)
            *out_x = x2 / pixel_width;
        else
            *out_x = (x2 / pixel_width) - 1;

        if (y2 % pixel_width)
            *out_y = y2 / pixel_width;
        else
            *out_y = (y2 / pixel_width) - 1;
    }
    else if (dx >= 0 && dy < 0)
    {
        if (x2 % pixel_width)
        {
            *out_x = x2 / pixel_width;
            *out_y = y2 / pixel_width;
        }
        else
        {
            *out_x = (x2 / pixel_width) - 1;
            *out_y = y2 / pixel_width;
        }
    }
    else if (dx < 0 && dy >= 0)
    {
        if (y2 % pixel_width)
        {
            *out_x = x2 / pixel_width;
            *out_y = y2 / pixel_width;
        }
        else
        {
            *out_x = x2 / pixel_width;
            *out_y = (y2 / pixel_width) - 1;
        }
    }
    else
    {
        *out_x = x2 / pixel_width;
        *out_y = y2 / pixel_width;
    }
}

bool verify_line_correct(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color, bool include_endpoints,
    uint32_t *image_pixels, int image_width, int image_height)
{
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            bool this_pixel_hit = line_intersects_pixel(x1, y1, x2, y2, x, y, pixel_width);
            // line_intersects_pixel only testes the infinite line, now perform bounds checking.
            int bounds_min_y = min(y1, y2) / pixel_width;
            int bounds_max_y = max(y1, y2) / pixel_width;
            int bounds_min_x = min(x1, x2) / pixel_width;
            int bounds_max_x = max(x1, x2) / pixel_width;
            bool in_bounds = x >= bounds_min_x && x <= bounds_max_x;
            in_bounds &= y >= bounds_min_y && y <= bounds_max_y;
            // Won't check anything that lies on the border. Its too annoying to check.
            // Probably can just let the manual tests check those cases.
            bool on_border = x == bounds_min_x && y >= bounds_min_y && y <= bounds_max_y;
            on_border |= x == bounds_max_x && y >= bounds_min_y && y <= bounds_max_y;
            on_border |= y == bounds_min_y && x >= bounds_min_x && x <= bounds_max_x;
            on_border |= y == bounds_max_y && x >= bounds_min_x && x <= bounds_max_x;
            if (x1 == x2 && y1 == y2)
                in_bounds = false;
            if (this_pixel_hit && in_bounds)
            {
                if (image_pixels[y * image_width + x] != color && !on_border)
                    return false; // There should be a pixel set here, but there isn't.
            }
            else if ((!this_pixel_hit || !in_bounds) && !on_border)
            {
                int32_t value = image_pixels[y * image_width + x];
                if (value == color)
                    return false; // There shouldn't be a pixel set here, but there is.
            }
        }
    }
    return true;
}

bool verify_line_recursion(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
    bool include_endpoints_in_lower, bool include_endpoints_in_upper,
    uint32_t *lower_pixels, int lower_width, int lower_height, uint32_t *upper_pixels)
{
    int upper_width = lower_width * 2;
    for (int y = 0; y < lower_height; y++)
    {
        for (int x = 0; x < lower_width; x++)
        {
            bool any_hit_in_upper = false;
            for (int y2 = y * 2; y2 < (y + 1) * 2; y2++)
            {
                for (int x2 = x * 2; x2 < (x + 1) * 2; x2++)
                {
                    if (upper_pixels[y2 * upper_width + x2] == color)
                        any_hit_in_upper = true;
                }
            }

            bool lower_is_endpoint = x == (x1 / pixel_width) && y == (y1 / pixel_width);
            lower_is_endpoint |= x == (x2 / pixel_width) && y == (y2 / pixel_width);

            // Don't think anything needs this calculation, but here it is anyways just in case.
            bool upper_is_endpoint = x == (x1 / (pixel_width * 2)) && y == (y1 / (pixel_width * 2));
            upper_is_endpoint |= x == (x2 / (pixel_width * 2)) && y == (y2 / (pixel_width * 2));

            // Won't bother to check the endpoints, again, its too much of a headache right now
            if (!lower_is_endpoint && !upper_is_endpoint)
            {
                if (any_hit_in_upper)
                {
                    if (lower_pixels[y * lower_width + x] != color)
                        return false; // There should be a pixel set here, but there isn't.
                }
                else
                {
                    int32_t value = lower_pixels[y * lower_width + x];
                    if (value == color)
                        return false; // There shouldn't be a pixel set here, but there is.
                }
            }
        }
    }
    return true;
}

void test_line_recursion(int x1, int y1, int x2, int y2, int pixel_width)
{
    int upper_width = 32;
    int upper_height = 32;
    int lower_width = upper_width / 2;
    int lower_height = upper_height / 2;
    uint32_t *lower_pixels = (uint32_t*)calloc(lower_width * lower_height, sizeof(uint32_t));
    uint32_t *upper_pixels = (uint32_t*)calloc(upper_width * upper_height, sizeof(uint32_t));
    uint32_t color = 1;
    drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    bool success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, false, false, lower_pixels, lower_width, lower_height, upper_pixels);
    EXPECT_TRUE(success);
    memset(lower_pixels, 0, lower_width * lower_height * sizeof(uint32_t));
    memset(upper_pixels, 0, upper_width * upper_height * sizeof(uint32_t));
    drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, false, true, lower_pixels, lower_width, lower_height, upper_pixels);
    EXPECT_TRUE(success);
    memset(lower_pixels, 0, lower_width * lower_height * sizeof(uint32_t));
    memset(upper_pixels, 0, upper_width * upper_height * sizeof(uint32_t));
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, true, false, lower_pixels, lower_width, lower_height, upper_pixels);
    EXPECT_TRUE(success);
    memset(lower_pixels, 0, lower_width * lower_height * sizeof(uint32_t));
    memset(upper_pixels, 0, upper_width * upper_height * sizeof(uint32_t));
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, true, true, lower_pixels, lower_width, lower_height, upper_pixels);
    EXPECT_TRUE(success);
    free(lower_pixels);
    free(upper_pixels);
}

void test_draw_line_basic(int x1, int y1, int x2, int y2, int pixel_width)
{
    int image_width = 32;
    int image_height = 32;
    uint32_t *image_pixels = (uint32_t*)calloc(image_width * image_height, sizeof(uint32_t));
    uint32_t color = 1;
            if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 1)
            {
                int g = 0;
            }
    drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width, color, image_pixels, image_width, image_height);
    bool success = verify_line_correct(x1, y1, x2, y2, pixel_width, color, false, image_pixels, image_width, image_height);
    EXPECT_TRUE(success);
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width, color, image_pixels, image_width, image_height);
    success = verify_line_correct(x1, y1, x2, y2, pixel_width, color, true, image_pixels, image_width, image_height);
    EXPECT_TRUE(success);
    free(image_pixels);
}

void test_line(int x1, int y1, int x2, int y2, int pixel_width)
{
    test_draw_line_basic(x1, y1, x2, y2, pixel_width);
    if (pixel_width > 1)
        test_line_recursion(x1, y1, x2, y2, pixel_width);
}

bool verify_line_manual(int x1, int y1, int x2, int y2, int pixel_width, bool include_endpoints,
    std::vector<std::pair<int, int>> known_correct_points)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int min_x = INT32_MAX;
    int max_x = 0;
    int min_y = INT32_MAX;
    int max_y = 0;
    for (std::pair<int, int> &point : known_correct_points)
    {
        min_x = min(min_x, point.first);
        max_x = max(max_x, point.first);
        min_y = min(min_y, point.second);
        max_y = max(max_y, point.second);
    }

    int width = max_x + 10;
    int height = max_y + 10;
    uint32_t *correct_pixels = (uint32_t*)calloc(width * height, sizeof(uint32_t));
    uint32_t *test_pixels = (uint32_t*)calloc(width * height, sizeof(uint32_t));

    uint32_t color = 1;
    for (std::pair<int, int> &point : known_correct_points)
    {
        int x = point.first;
        int y = point.second;
        correct_pixels[y * width + x] = color;
    }

    if (include_endpoints)
        drawline_include_endpoints(x1, y1, x2, y2, pixel_width, color, test_pixels, width, height);
    else
        drawline_exclude_endpoints(x1, y1, x2, y2, pixel_width, color, test_pixels, width, height);
    bool success = true;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint32_t test_val = test_pixels[y * width + x];
            uint32_t correct_val = correct_pixels[y * width + x];
            if (test_val != correct_val)
                success = false;
        }
    }
    free(correct_pixels);
    free(test_pixels);
    return success;
}

// Only tests very simple 45 degree going (+,+)
TEST(diagonal_top_right, DrawLine)
{
    /*
    test_line(0, 0, 16, 16, 1);
    test_line(0, 0, 16, 16, 2);
    test_line(0, 0, 16, 16, 4);
    test_line(0, 0, 16, 16, 8);

    test_line(0, 3, 16, 16, 1);
    test_line(0, 3, 16, 16, 2);
    test_line(0, 3, 16, 16, 4);
    test_line(0, 3, 16, 16, 8);

    test_line(3, 3, 16, 16, 1);
    test_line(3, 3, 16, 16, 2);
    test_line(3, 3, 16, 16, 4);
    test_line(3, 3, 16, 16, 8);
    */
}

TEST(auto_test_top_right, DrawLine)
{
    /*
    for (int width = 2; width < 16; width *= 2)
    {
        for (int x2 = 0; x2 < 32; x2++)
        {
            for (int y2 = 0; y2 < 32; y2++)
            {
                int x1 = 0;
                int y1 = 0;
                test_line(x1, y1, x2, y2, width);
            }
        }
    }
    */
}



// Several known correct points on the line.
// These points are calculated manually, by looking at a graph to see exactly which squares intersect the line.
// This is where the actual behaviour is proven to be what is needed.
TEST(manual_test_bottom_right, DrawLine)
{
    int x1, y1, x2, y2;
    int pixel_width;
    bool success;
    x1 = 0;
    y1 = 7;
    x2 = 7;
    y2 = 0;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 6),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 0)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 6),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 0)
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 6),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 0)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 6),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(5, 1)
    });
    EXPECT_FALSE(success);

    x1 = 3;
    y1 = 17;
    x2 = 16;
    y2 = 7;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 16),
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
        std::pair<int, int>(15, 7),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
        std::pair<int, int>(15, 7),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 16),
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 16),
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
        std::pair<int, int>(15, 7),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 16),
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
        std::pair<int, int>(15, 7),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 16),
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 16),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(5, 15),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(6, 14),
        std::pair<int, int>(6, 13),
        std::pair<int, int>(7, 13),
        std::pair<int, int>(8, 13),
        std::pair<int, int>(8, 12),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(10, 11),
        std::pair<int, int>(10, 10),
        std::pair<int, int>(11, 10),
        std::pair<int, int>(12, 10),
        std::pair<int, int>(12, 9),
        std::pair<int, int>(13, 9),
        std::pair<int, int>(13, 8),
        std::pair<int, int>(14, 8),
        std::pair<int, int>(14, 7),
        std::pair<int, int>(15, 7)
    });
    EXPECT_FALSE(success);



    x1 = 3;
    y1 = 17;
    x2 = 16;
    y2 = 7;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,8),
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,8),
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);



    x1 = 4;
    y1 = 17;
    x2 = 16;
    y2 = 7;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);


    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        //std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        //std::pair<int, int>(7,3),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,8),
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,7),
        std::pair<int, int>(3,7),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(5,5),
        std::pair<int, int>(6,5),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3)
    });
    EXPECT_FALSE(success);


    x1 = 3;
    y1 = 16;
    x2 = 16;
    y2 = 7;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,7),
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,7),
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,7),
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
        std::pair<int, int>(7,3),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,7),
        std::pair<int, int>(2,7),
        std::pair<int, int>(2,6),
        std::pair<int, int>(3,6),
        std::pair<int, int>(4,6),
        std::pair<int, int>(4,5),
        std::pair<int, int>(5,5),
        std::pair<int, int>(5,4),
        std::pair<int, int>(6,4),
        std::pair<int, int>(7,4),
    });
    EXPECT_FALSE(success);



    x1 = 5;
    y1 = 90;
    x2 = 69;
    y2 = 74;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);



    x1 = 4;
    y1 = 90;
    x2 = 69;
    y2 = 74;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);


    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);



    x1 = 7;
    y1 = 90;
    x2 = 69;
    y2 = 74;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,22),
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,22),
        std::pair<int, int>(3,22),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(6,21),
        std::pair<int, int>(7,21),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(11,20),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(15,19),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_TRUE(success);


    x1 = 4;
    y1 = 88;
    x2 = 69;
    y2 = 74;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(10,20),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(14,19),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_TRUE(success);



    x1 = 4;
    y1 = 88;
    x2 = 69;
    y2 = 72;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
        std::pair<int, int>(17,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18)
    });
    EXPECT_TRUE(success);



    x1 = 4;
    y1 = 88;
    x2 = 68;
    y2 = 72;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1,21),
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
        std::pair<int, int>(16,18),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2,21),
        std::pair<int, int>(3,21),
        std::pair<int, int>(4,21),
        std::pair<int, int>(5,20),
        std::pair<int, int>(6,20),
        std::pair<int, int>(7,20),
        std::pair<int, int>(8,20),
        std::pair<int, int>(9,19),
        std::pair<int, int>(10,19),
        std::pair<int, int>(11,19),
        std::pair<int, int>(12,19),
        std::pair<int, int>(13,18),
        std::pair<int, int>(14,18),
        std::pair<int, int>(15,18),
    });
    EXPECT_TRUE(success);



    x1 = 0;
    y1 = 64;
    x2 = 0;
    y2 = 16;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,15),
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
        std::pair<int, int>(0,4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
    });
    EXPECT_TRUE(success);


    x1 = 0;
    y1 = 63;
    x2 = 0;
    y2 = 16;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,15),
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
        std::pair<int, int>(0,4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
    });
    EXPECT_TRUE(success);


    x1 = 0;
    y1 = 63;
    x2 = 0;
    y2 = 17;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,15),
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
        std::pair<int, int>(0,4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0,14),
        std::pair<int, int>(0,13),
        std::pair<int, int>(0,12),
        std::pair<int, int>(0,11),
        std::pair<int, int>(0,10),
        std::pair<int, int>(0,9),
        std::pair<int, int>(0,8),
        std::pair<int, int>(0,7),
        std::pair<int, int>(0,6),
        std::pair<int, int>(0,5),
    });
    EXPECT_TRUE(success);
}


// Several known correct points on the line.
// These points are calculated manually, by looking at a graph to see exactly which squares intersect the line.
// This is where the actual behaviour is proven to be what is needed.
TEST(manual_test_top_right, DrawLine)
{
    int x1, y1, x2, y2;
    int pixel_width;
    bool success;

    x1 = 0;
    y1 = 0;
    x2 = 5;
    y2 = 7;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(4, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(4, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(4, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
        //std::pair<int, int>(4, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(3, 5),
        std::pair<int, int>(4, 5),
        //std::pair<int, int>(4, 6)
    });
    EXPECT_FALSE(success);
    
    x1 = 0;
    y1 = 0;
    x2 = 16;
    y2 = 3;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(5, 0),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(7, 1),
        std::pair<int, int>(8, 1),
        std::pair<int, int>(9, 1),
        std::pair<int, int>(10, 1),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(11, 2),
        std::pair<int, int>(12, 2),
        std::pair<int, int>(13, 2),
        std::pair<int, int>(14, 2),
        std::pair<int, int>(15, 2)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(5, 0),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(7, 1),
        std::pair<int, int>(8, 1),
        std::pair<int, int>(9, 1),
        std::pair<int, int>(10, 1),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(11, 2),
        std::pair<int, int>(12, 2),
        std::pair<int, int>(13, 2),
        std::pair<int, int>(14, 2),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(5, 0),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(7, 1),
        std::pair<int, int>(8, 1),
        std::pair<int, int>(9, 1),
        std::pair<int, int>(10, 1),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(11, 2),
        std::pair<int, int>(12, 2),
        std::pair<int, int>(13, 2),
        std::pair<int, int>(14, 2),
        std::pair<int, int>(15, 2),
        std::pair<int, int>(16, 3),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(5, 0),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(7, 1),
        std::pair<int, int>(8, 1),
        std::pair<int, int>(9, 1),
        std::pair<int, int>(10, 1),
        std::pair<int, int>(10, 2),
        std::pair<int, int>(11, 2),
        std::pair<int, int>(12, 2),
        std::pair<int, int>(13, 2),
        std::pair<int, int>(14, 2),
        //std::pair<int, int>(15, 2),
    });
    EXPECT_TRUE(success);

    x1 = 5;
    y1 = 1;
    x2 = 16;
    y2 = 7;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2),
        std::pair<int, int>(8, 3),
        std::pair<int, int>(9, 3),
        std::pair<int, int>(10, 3),
        std::pair<int, int>(10, 4),
        std::pair<int, int>(11, 4),
        std::pair<int, int>(12, 4),
        std::pair<int, int>(12, 5),
        std::pair<int, int>(13, 5),
        std::pair<int, int>(14, 5),
        std::pair<int, int>(14, 6),
        std::pair<int, int>(15, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2),
        std::pair<int, int>(8, 3),
        std::pair<int, int>(9, 3),
        std::pair<int, int>(10, 3),
        std::pair<int, int>(10, 4),
        std::pair<int, int>(11, 4),
        std::pair<int, int>(12, 4),
        std::pair<int, int>(12, 5),
        std::pair<int, int>(13, 5),
        std::pair<int, int>(14, 5),
        std::pair<int, int>(14, 6),
        std::pair<int, int>(15, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2),
        std::pair<int, int>(8, 3),
        std::pair<int, int>(9, 3),
        std::pair<int, int>(10, 3),
        std::pair<int, int>(10, 4),
        std::pair<int, int>(11, 4),
        std::pair<int, int>(12, 4),
        std::pair<int, int>(12, 5),
        std::pair<int, int>(13, 5),
        std::pair<int, int>(14, 5),
        std::pair<int, int>(14, 6),
        //std::pair<int, int>(15, 6)
    });
    EXPECT_TRUE(success);
  
    x1 = 10;
    y1 = 1;
    x2 = 20;
    y2 = 1;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(10, 1),
        std::pair<int, int>(11, 1),
        std::pair<int, int>(12, 1),
        std::pair<int, int>(13, 1),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(15, 1),
        std::pair<int, int>(16, 1),
        std::pair<int, int>(17, 1),
        std::pair<int, int>(18, 1),
        std::pair<int, int>(19, 1)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(11, 1),
        std::pair<int, int>(12, 1),
        std::pair<int, int>(13, 1),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(15, 1),
        std::pair<int, int>(16, 1),
        std::pair<int, int>(17, 1),
        std::pair<int, int>(18, 1),
        std::pair<int, int>(19, 1)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(11, 1),
        std::pair<int, int>(12, 1),
        std::pair<int, int>(13, 1),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(15, 1),
        std::pair<int, int>(16, 1),
        std::pair<int, int>(17, 1),
        std::pair<int, int>(18, 1),
        //std::pair<int, int>(19, 1)
    });
    EXPECT_TRUE(success);
    
    x1 = 1;
    y1 = 10;
    x2 = 1;
    y2 = 20;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 10),
        std::pair<int, int>(1, 11),
        std::pair<int, int>(1, 12),
        std::pair<int, int>(1, 13),
        std::pair<int, int>(1, 14),
        std::pair<int, int>(1, 15),
        std::pair<int, int>(1, 16),
        std::pair<int, int>(1, 17),
        std::pair<int, int>(1, 18),
        std::pair<int, int>(1, 19)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 11),
        std::pair<int, int>(1, 12),
        std::pair<int, int>(1, 13),
        std::pair<int, int>(1, 14),
        std::pair<int, int>(1, 15),
        std::pair<int, int>(1, 16),
        std::pair<int, int>(1, 17),
        std::pair<int, int>(1, 18),
        std::pair<int, int>(1, 19)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 11),
        std::pair<int, int>(1, 12),
        std::pair<int, int>(1, 13),
        std::pair<int, int>(1, 14),
        std::pair<int, int>(1, 15),
        std::pair<int, int>(1, 16),
        std::pair<int, int>(1, 17),
        std::pair<int, int>(1, 18),
        //std::pair<int, int>(1, 19)
    });
    EXPECT_TRUE(success);
    
    x1 = 3;
    y1 = 7;
    x2 = 16;
    y2 = 17;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 7),
        std::pair<int, int>(4, 7),
        std::pair<int, int>(4, 8),
        std::pair<int, int>(5, 8),
        std::pair<int, int>(5, 9),
        std::pair<int, int>(6, 9),
        std::pair<int, int>(6, 10),
        std::pair<int, int>(7, 10),
        std::pair<int, int>(8, 10),
        std::pair<int, int>(8, 11),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(10, 12),
        std::pair<int, int>(10, 13),
        std::pair<int, int>(11, 13),
        std::pair<int, int>(12, 13),
        std::pair<int, int>(12, 14),
        std::pair<int, int>(13, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(14, 15),
        std::pair<int, int>(14, 16),
        std::pair<int, int>(15, 16)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 7),
        std::pair<int, int>(4, 8),
        std::pair<int, int>(5, 8),
        std::pair<int, int>(5, 9),
        std::pair<int, int>(6, 9),
        std::pair<int, int>(6, 10),
        std::pair<int, int>(7, 10),
        std::pair<int, int>(8, 10),
        std::pair<int, int>(8, 11),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(10, 12),
        std::pair<int, int>(10, 13),
        std::pair<int, int>(11, 13),
        std::pair<int, int>(12, 13),
        std::pair<int, int>(12, 14),
        std::pair<int, int>(13, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(14, 15),
        std::pair<int, int>(14, 16),
        std::pair<int, int>(15, 16)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 7),
        std::pair<int, int>(4, 8),
        std::pair<int, int>(5, 8),
        std::pair<int, int>(5, 9),
        std::pair<int, int>(6, 9),
        std::pair<int, int>(6, 10),
        std::pair<int, int>(7, 10),
        std::pair<int, int>(8, 10),
        std::pair<int, int>(8, 11),
        std::pair<int, int>(9, 11),
        std::pair<int, int>(9, 12),
        std::pair<int, int>(10, 12),
        std::pair<int, int>(10, 13),
        std::pair<int, int>(11, 13),
        std::pair<int, int>(12, 13),
        std::pair<int, int>(12, 14),
        std::pair<int, int>(13, 14),
        std::pair<int, int>(13, 15),
        std::pair<int, int>(14, 15),
        std::pair<int, int>(14, 16),
        //std::pair<int, int>(15, 16)
    });
    EXPECT_TRUE(success);

    x1 = 0;
    y1 = 0;
    x2 = 13;
    y2 = 13;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5)
    });
    EXPECT_TRUE(success);

    x1 = 0;
    y1 = 0;
    x2 = 12;
    y2 = 12;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        //std::pair<int, int>(5, 5)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        //std::pair<int, int>(5, 5)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5),
        //std::pair<int, int>(6, 6)
    });
    EXPECT_FALSE(success);

    x1 = 0;
    y1 = 0;
    x2 = 15;
    y2 = 13;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6)
    });
    EXPECT_FALSE(success);


    x1 = 0;
    y1 = 0;
    x2 = 17;
    y2 = 12;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(6, 4),
        std::pair<int, int>(7, 4),
        std::pair<int, int>(7, 5),
        std::pair<int, int>(8, 5)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(6, 4),
        std::pair<int, int>(7, 4),
        std::pair<int, int>(7, 5),
        std::pair<int, int>(8, 5)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(6, 4),
        std::pair<int, int>(7, 4),
        std::pair<int, int>(7, 5),
        std::pair<int, int>(8, 5),
        std::pair<int, int>(8, 6)
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(6, 4),
        std::pair<int, int>(7, 4),
        std::pair<int, int>(7, 5),
        //std::pair<int, int>(8, 5)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(6, 4),
        std::pair<int, int>(7, 4),
        std::pair<int, int>(7, 5),
        std::pair<int, int>(8, 6)
    });
    EXPECT_FALSE(success);


    x1 = 0;
    y1 = 0;
    x2 = 17;
    y2 = 5;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2),
        std::pair<int, int>(9, 2),
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2),
        std::pair<int, int>(8, 2),
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(4, 1),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(6, 1),
        std::pair<int, int>(6, 2),
        std::pair<int, int>(7, 2)
    });
    EXPECT_FALSE(success);

    x1 = 4;
    y1 = 7;
    x2 = 17;
    y2 = 15;
    pixel_width = 2;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7),
        std::pair<int, int>(8, 7)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7)
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7),
        std::pair<int, int>(8, 7)
    });
    EXPECT_FALSE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7)
    });
    EXPECT_TRUE(success);

    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7),
        std::pair<int, int>(8, 7),
        
    });
    EXPECT_FALSE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(3, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 5),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 5),
        std::pair<int, int>(6, 6),
        std::pair<int, int>(7, 6),
        std::pair<int, int>(7, 7),
        
    });
    EXPECT_FALSE(success);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
