
#include <stdlib.h>
#include <utility>
#include <vector>
#include <functional>
#include "gtest/gtest.h"
#include "draw_line.h"
#include "line_traverser.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

enum hit_type
{
    hit, unsure, miss
};

hit_type line_square_intersection(int x1, int y1, int x2, int y2, int square_x, int square_y, int square_width)
{
    square_x *= square_width;
    square_y *= square_width;
    if (x1 == x2)
    {
        if (x1 > square_x && x1 < square_x + square_width)
        {
            if (min(y1, y2) < square_y + square_width && max(y1, y2) > square_y)
                return hit;
            else
                return unsure;
        }
        else if (x1 < square_x || x1 > square_x + square_width)
        {
            return miss;
        }
        else
        {
            return unsure;
        }
    }
    if (y1 == y2)
    {
        if (y1 > square_y && y1 < square_y + square_width)
        {
            if (min(x1, x2) < square_x + square_width && max(x1, x2) > square_x)
                return hit;
            else
                return unsure;
        }
        else if (y1 < square_y || y1 > square_y + square_width)
        {
            return miss;
        }
        else
        {
            return unsure;
        }
    }

    double dx = (double)(x2 - x1);
    double dy = (double)(y2 - y1);
    double rcp_dx = 1.0 / dx;
    double rcp_dy = 1.0 / dy;
    double t1x = ((double)((dx >= 0.0) ? (square_x) : (square_x + square_width)) - (double)x1) * rcp_dx;
    double t1y = ((double)((dy >= 0.0) ? (square_y) : (square_y + square_width)) - (double)y1) * rcp_dy;
    double t2x = ((double)((dx >= 0.0) ? (square_x + square_width) : (square_x)) - (double)x1) * rcp_dx;
    double t2y = ((double)((dy >= 0.0) ? (square_y + square_width) : (square_y)) - (double)y1) * rcp_dy;

    double t_min = max(t1x, t1y);
    double t_max = min(t2x, t2y);
    if (t_min < t_max - 0.00001)
    {
        if (t_max < 0.0001 || t_min > 0.999)
            return miss;
        return hit;
    }
    else if (t_min > t_max + 0.00001)
        return miss;
    else
    {
        if (t_max < 0.0001 || t_min > 0.999)
            return miss;
        return unsure;
    }
}

hit_type square_is_on_endpoint(int x1, int y1, int x2, int y2, int square_x, int square_y, int square_width)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x1_scaled = x1 / square_width;
    int y1_scaled = y1 / square_width;
    int x2_scaled = x2 / square_width;
    int y2_scaled = y2 / square_width;
    if (dy < 0 && (y1 % square_width) == 0)
        y1_scaled--;
    if (dx < 0 && (x1 % square_width) == 0)
        x1_scaled--;

    if (dy > 0 && (y2 % square_width) == 0)
        y2_scaled--;
    if (dx > 0 && (x2 % square_width) == 0)
        x2_scaled--;
    
    if (square_x == x1_scaled && square_y == y1_scaled)
            return hit;

    if (square_x == x2_scaled && square_y == y2_scaled)
            return hit;
    return miss;
}

bool verify_line_correct(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color, bool include_endpoints,
    uint32_t *image_pixels, int image_width, int image_height)
{
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            hit_type pixel_hit = line_square_intersection(x1, y1, x2, y2, x, y, pixel_width);
            hit_type is_endpoint = square_is_on_endpoint(x1, y1, x2, y2, x, y, pixel_width);
            if (is_endpoint == hit && !include_endpoints)
                pixel_hit = miss;
            if (pixel_hit == hit)
            {
                if (image_pixels[y * image_width + x] != color)
                    return false; // There should be a pixel set here, but there isn't.
            }
            else if (pixel_hit == miss)
            {
                uint32_t value = image_pixels[y * image_width + x];
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
                    uint32_t value = lower_pixels[y * lower_width + x];
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
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline_include_endpoints(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    bool success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, true, true, lower_pixels, lower_width, lower_height, upper_pixels);
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

TEST(auto_test, DrawLine)
{
    for (int width = 2; width < 16; width *= 2)
    {
        for (int x2 = 0; x2 < 64; x2++)
        {
            for (int y2 = 0; y2 < 64; y2++)
            {
                int x1 = 32;
                int y1 = 32;
                test_line(x1, y1, x2, y2, width);
            }
        }
    }
}


// Several known correct points on the line.
// These points are calculated manually, by looking at a graph to see exactly which squares intersect the line.
// This is where the actual behaviour is proven to be what is needed.
TEST(manual_test_bottom_left, DrawLine)
{
    int x1, y1, x2, y2;
    int pixel_width;
    bool success;
    x1 = 7;
    y1 = 7;
    x2 = 0;
    y2 = 0;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 6),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(0, 0)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 5),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 1),
    });
    EXPECT_TRUE(success);

    x1 = 25;
    y1 = 18;
    x2 = 1;
    y2 = 9;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(0, 2),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
    });
    EXPECT_TRUE(success);


    x1 = 25;
    y1 = 18;
    x2 = 1;
    y2 = 9;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 4),
        std::pair<int, int>(5, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(0, 2),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 4),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
    });
    EXPECT_TRUE(success);


    x1 = 24;
    y1 = 18;
    x2 = 9;
    y2 = 9;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 4),
        std::pair<int, int>(5, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(2, 2),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 3),
        std::pair<int, int>(4, 3),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(3, 2),
    });
    EXPECT_TRUE(success);
}

// Several known correct points on the line.
// These points are calculated manually, by looking at a graph to see exactly which squares intersect the line.
// This is where the actual behaviour is proven to be what is needed.
TEST(manual_test_top_left, DrawLine)
{
    int x1, y1, x2, y2;
    int pixel_width;
    bool success;
    x1 = 7;
    y1 = 0;
    x2 = 0;
    y2 = 7;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(6, 0),
        std::pair<int, int>(5, 1),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(0, 6)
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 1),
        std::pair<int, int>(4, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(2, 4),
        std::pair<int, int>(1, 5)
    });
    EXPECT_TRUE(success);


    x1 = 16;
    y1 = 1;
    x2 = 1;
    y2 = 19;
    pixel_width = 1;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(15, 1),
        std::pair<int, int>(15, 2),
        std::pair<int, int>(14, 2),
        std::pair<int, int>(14, 3),
        std::pair<int, int>(13, 3),
        std::pair<int, int>(13, 4),
        std::pair<int, int>(12, 4),
        std::pair<int, int>(12, 5),
        std::pair<int, int>(11, 5),
        std::pair<int, int>(11, 6),
        std::pair<int, int>(10, 7),
        std::pair<int, int>(10, 8),
        std::pair<int, int>(9, 8),
        std::pair<int, int>(9, 9),
        std::pair<int, int>(8, 9),
        std::pair<int, int>(8, 10),
        std::pair<int, int>(7, 10),
        std::pair<int, int>(7, 11),
        std::pair<int, int>(6, 11),
        std::pair<int, int>(6, 12),
        std::pair<int, int>(5, 13),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(4, 14),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(3, 15),
        std::pair<int, int>(3, 16),
        std::pair<int, int>(2, 16),
        std::pair<int, int>(2, 17),
        std::pair<int, int>(1, 17),
        std::pair<int, int>(1, 18),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(15, 2),
        std::pair<int, int>(14, 2),
        std::pair<int, int>(14, 3),
        std::pair<int, int>(13, 3),
        std::pair<int, int>(13, 4),
        std::pair<int, int>(12, 4),
        std::pair<int, int>(12, 5),
        std::pair<int, int>(11, 5),
        std::pair<int, int>(11, 6),
        std::pair<int, int>(10, 7),
        std::pair<int, int>(10, 8),
        std::pair<int, int>(9, 8),
        std::pair<int, int>(9, 9),
        std::pair<int, int>(8, 9),
        std::pair<int, int>(8, 10),
        std::pair<int, int>(7, 10),
        std::pair<int, int>(7, 11),
        std::pair<int, int>(6, 11),
        std::pair<int, int>(6, 12),
        std::pair<int, int>(5, 13),
        std::pair<int, int>(5, 14),
        std::pair<int, int>(4, 14),
        std::pair<int, int>(4, 15),
        std::pair<int, int>(3, 15),
        std::pair<int, int>(3, 16),
        std::pair<int, int>(2, 16),
        std::pair<int, int>(2, 17),
        std::pair<int, int>(1, 17),
    });
    EXPECT_TRUE(success);


    x1 = 16;
    y1 = 1;
    x2 = 1;
    y2 = 19;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 0),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(0, 3),
        std::pair<int, int>(0, 4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 1),
        std::pair<int, int>(2, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(0, 3),
    });
    EXPECT_TRUE(success);

    x1 = 16;
    y1 = 4;
    x2 = 1;
    y2 = 19;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(0, 4),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(2, 2),
        std::pair<int, int>(1, 3),
    });
    EXPECT_TRUE(success);

    x1 = 18;
    y1 = 4;
    x2 = 1;
    y2 = 19;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 1),
        std::pair<int, int>(3, 1),
        std::pair<int, int>(3, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(1, 4),
        std::pair<int, int>(0, 4),
    });
    EXPECT_TRUE(success);


    x1 = 20;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 21;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);
    
    x1 = 22;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 23;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 20;
    y1 = 4;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 21;
    y1 = 4;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);
    
    x1 = 22;
    y1 = 4;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 23;
    y1 = 4;
    x2 = 0;
    y2 = 0;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 20;
    y1 = 3;
    x2 = 0;
    y2 = 3;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 21;
    y1 = 3;
    x2 = 0;
    y2 = 3;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);
    
    x1 = 22;
    y1 = 3;
    x2 = 0;
    y2 = 3;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);

    x1 = 23;
    y1 = 3;
    x2 = 0;
    y2 = 3;
    pixel_width = 4;
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, true, std::vector<std::pair<int, int>>{
        std::pair<int, int>(5, 0),
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
        std::pair<int, int>(0, 0),
    });
    EXPECT_TRUE(success);
    success = verify_line_manual(x1, y1, x2, y2, pixel_width, false, std::vector<std::pair<int, int>>{
        std::pair<int, int>(4, 0),
        std::pair<int, int>(3, 0),
        std::pair<int, int>(2, 0),
        std::pair<int, int>(1, 0),
    });
    EXPECT_TRUE(success);
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

typedef struct
{
    int i;
    int32_t ep1x, ep1y, ep2x, ep2y;
} VerifyEndpointsInfo;


void verify_endpoints_callback(int32_t x, int32_t y, void *user_data)
{
    VerifyEndpointsInfo *p_info = (VerifyEndpointsInfo*)user_data;
    if (p_info->i == 0)
    {
        p_info->ep1x = x;
        p_info->ep1y = y;
    }
    p_info->i++;
    p_info->ep2x = x;
    p_info->ep2y = y;
}

bool verify_endpoints(int x1, int y1, int x2, int y2, int square_width)
{
    int32_t test_ep1x, test_ep1y, test_ep2x, test_ep2y;
    VerifyEndpointsInfo info;
    info.i = 0;
    LineTraverser_get_endpoints(x1, y1, x2, y2, square_width, &test_ep1x, &test_ep1y,
    &test_ep2x, &test_ep2y);
    LineTraverser_traverse_include_endpoints(x1,y1,x2,y2,square_width, verify_endpoints_callback, &info);
    if (info.ep1x != test_ep1x)
        return false;
    if (info.ep1y != test_ep1y)
        return false;
    if (info.ep2x != test_ep2x)
        return false;
    if (info.ep2y != test_ep2y)
        return false;
    return true;
}

TEST(endpoint_test, DrawLine)
{
    int32_t x1, y1, x2, y2, square_width;
    bool include_endpoints;
    bool success;

    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 100;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 0;
    y1 = 100;
    x2 = 0;
    y2 = 0;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 0;
    y1 = 0;
    x2 = 100;
    y2 = 0;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 1000;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);


    x1 = 1;
    y1 = 0;
    x2 = 1;
    y2 = 100;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 1;
    y1 = 100;
    x2 = 1;
    y2 = 0;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);


    x1 = 0;
    y1 = 1;
    x2 = 100;
    y2 = 1;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 1000;
    y1 = 1;
    x2 = 0;
    y2 = 1;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);


    x1 = 0;
    y1 = 0;
    x2 = 100;
    y2 = 100;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    x1 = 100;
    y1 = 100;
    x2 = 0;
    y2 = 0;
    verify_endpoints(x1, y1, x2, y2, 1);
    verify_endpoints(x1, y1, x2, y2, 2);
    verify_endpoints(x1, y1, x2, y2, 4);
    verify_endpoints(x1, y1, x2, y2, 8);

    for (int i = 0; i < 1000; i++)
    {
        for (int square_width = 1; square_width < 64; square_width *= 2)
        {
            x1 = rand() % 1024;
            y1 = rand() % 1024;
            x2 = rand() % 1024;
            y2 = rand() % 1024;
            success = verify_endpoints(x1, y1, x2, y2, square_width);
        }
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
