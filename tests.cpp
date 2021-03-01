
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

void rightmost_point_on_square_to_line(int x1, int y1, int x2, int y2, int square_x, int square_y, int square_width,
    int *out_x, int *out_y)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int normal_x = dy;
    int normal_y = -dx;
    *out_x = (normal_x >= 0) ? (square_x) : (square_x + square_width - 1);
    *out_y = (normal_y >= 0) ? (square_y) : (square_y + square_width - 1);
}
void leftmost_point_on_square_to_line(int x1, int y1, int x2, int y2, int square_x, int square_y, int square_width,
    int *out_x, int *out_y)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int normal_x = dy;
    int normal_y = -dx;
    *out_x = (normal_x < 0) ? (square_x) : (square_x + square_width - 0);
    *out_y = (normal_y < 0) ? (square_y) : (square_y + square_width - 0);
}

bool line_intersects_pixel_clockwiseness_test(int x1, int y1, int x2, int y2, int pixel_x, int pixel_y, int pixel_width)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int left_x, left_y, right_x, right_y;
    leftmost_point_on_square_to_line(x1, y1, x2, y2, pixel_x, pixel_y, pixel_width, &left_x, &left_y);
    rightmost_point_on_square_to_line(x1, y1, x2, y2, pixel_x, pixel_y, pixel_width, &right_x, &right_y);
    int left_clockwiseness = dx * (left_y - y1) - dy * (left_x - x1);
    int right_clockwiseness = dx * (right_y - y1) - dy * (right_x - x1);

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

    if (left_clockwiseness == right_clockwiseness && left_clockwiseness == 0)
        return true;

    if (left_clockwiseness <= 0)
        return false;
    if (right_clockwiseness >= 0)
        return false;
    return true;
}

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

bool verify_line_correct(int x1, int y1, int x2, int y2, int pixel_width, uint32_t color,
    uint32_t *image_pixels, int image_width, int image_height)
{
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            if (y == 1 && x == 1 && x1 == 0 && y1 == 0 && x2 == 16 && y2 == 16)
            {
                int g = 0;
            }
            bool this_pixel_hit = line_intersects_pixel(x1, y1, x2, y2, x, y, pixel_width);
            bool clockwiseness_result = line_intersects_pixel_clockwiseness_test(x1, y1, x2, y2, x, y, pixel_width);
            if (clockwiseness_result != this_pixel_hit && pixel_width > 1)
            {
                int g = 0;
            }
            // line_intersects_pixel only testes the infinite line, now perform bounds checking.
            bool in_bounds = x >= (min(x1, x2) / pixel_width) && x <= (max(x1, x2) / pixel_width);
            in_bounds &= y >= (min(y1, y2) / pixel_width) && y <= (max(y1, y2) / pixel_width);
            if (this_pixel_hit && in_bounds)
            {
                if (image_pixels[y * image_width + x] != color)
                    return false; // There should be a pixel set here, but there isn't.
            }
            else if (!this_pixel_hit || !in_bounds)
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
    drawline(x1, y1, x2, y2, pixel_width, color, upper_pixels, upper_width, upper_height);
    drawline(x1, y1, x2, y2, pixel_width * 2, color, lower_pixels, lower_width, lower_height);
    bool success = verify_line_recursion(x1, y1, x2, y2, pixel_width, color, lower_pixels, lower_width, lower_height, upper_pixels);
    free(lower_pixels);
    free(upper_pixels);
    EXPECT_TRUE(success);
}

void test_draw_line_basic(int x1, int y1, int x2, int y2, int pixel_width)
{
    int image_width = 32;
    int image_height = 32;
    uint32_t *image_pixels = (uint32_t*)calloc(image_width * image_height, sizeof(uint32_t));
    uint32_t color = 1;
    drawline(x1, y1, x2, y2, pixel_width, color, image_pixels, image_width, image_height);
    bool success = verify_line_correct(x1, y1, x2, y2, pixel_width, color, image_pixels, image_width, image_height);
    free(image_pixels);
    EXPECT_TRUE(success);
}

void test_line(int x1, int y1, int x2, int y2, int pixel_width)
{
    test_draw_line_basic(x1, y1, x2, y2, pixel_width);
    if (pixel_width > 1)
        test_line_recursion(x1, y1, x2, y2, pixel_width);
}

void verify_line_manual(int x1, int y1, int x2, int y2, int pixel_width,
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

    int width = max_x + 1;
    int height = max_y + 1;
    uint32_t *correct_pixels = (uint32_t*)calloc(width * height, sizeof(uint32_t));
    uint32_t *test_pixels = (uint32_t*)calloc(width * height, sizeof(uint32_t));

    uint32_t color = 1;
    for (std::pair<int, int> &point : known_correct_points)
    {
        int x = point.first;
        int y = point.second;
        correct_pixels[y * width + x] = color;
    }

    drawline(x1, y1, x2, y2, pixel_width, color, test_pixels, width, height);
    bool success = true;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (test_pixels[y * width + x] != correct_pixels[y * width + x])
                success = false;
        }
    }

    free(correct_pixels);
    free(test_pixels);
    EXPECT_TRUE(success);
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
TEST(manual_test_top_right, DrawLine)
{
    int x1, y1, x2, y2;
    int pixel_width;


    x1 = 0;
    y1 = 0;
    x2 = 5;
    y2 = 7;
    pixel_width = 1;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(1, 2),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(2, 3),
    });

    x1 = 0;
    y1 = 0;
    x2 = 16;
    y2 = 3;
    pixel_width = 1;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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
        std::pair<int, int>(15, 2),
        std::pair<int, int>(16, 3),
    });


    x1 = 5;
    y1 = 1;
    x2 = 16;
    y2 = 7;
    pixel_width = 1;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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
        std::pair<int, int>(15, 6),
        std::pair<int, int>(16, 7),
    });


    x1 = 10;
    y1 = 1;
    x2 = 20;
    y2 = 1;
    pixel_width = 1;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
        std::pair<int, int>(10, 1),
        std::pair<int, int>(11, 1),
        std::pair<int, int>(12, 1),
        std::pair<int, int>(13, 1),
        std::pair<int, int>(14, 1),
        std::pair<int, int>(15, 1),
        std::pair<int, int>(16, 1),
        std::pair<int, int>(17, 1),
        std::pair<int, int>(18, 1),
        std::pair<int, int>(19, 1),
        std::pair<int, int>(20, 1)
    });

    x1 = 1;
    y1 = 10;
    x2 = 1;
    y2 = 20;
    pixel_width = 1;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
        std::pair<int, int>(1, 10),
        std::pair<int, int>(1, 11),
        std::pair<int, int>(1, 12),
        std::pair<int, int>(1, 13),
        std::pair<int, int>(1, 14),
        std::pair<int, int>(1, 15),
        std::pair<int, int>(1, 16),
        std::pair<int, int>(1, 17),
        std::pair<int, int>(1, 18),
        std::pair<int, int>(1, 19),
        std::pair<int, int>(1, 20)
    });

    x1 = 0;
    y1 = 0;
    x2 = 13;
    y2 = 13;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(1, 1),
        std::pair<int, int>(2, 2),
        std::pair<int, int>(3, 3),
        std::pair<int, int>(4, 4),
        std::pair<int, int>(5, 5),
        std::pair<int, int>(6, 6)
    });

    x1 = 0;
    y1 = 0;
    x2 = 15;
    y2 = 13;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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

    x1 = 0;
    y1 = 0;
    x2 = 17;
    y2 = 12;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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

    x1 = 0;
    y1 = 0;
    x2 = 17;
    y2 = 5;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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
    

    x1 = 0;
    y1 = 0;
    x2 = 5;
    y2 = 17;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
        std::pair<int, int>(0, 0),
        std::pair<int, int>(0, 1),
        std::pair<int, int>(0, 2),
        std::pair<int, int>(0, 3),
        std::pair<int, int>(1, 3),
        std::pair<int, int>(1, 4),
        std::pair<int, int>(1, 5),
        std::pair<int, int>(1, 6),
        std::pair<int, int>(2, 6),
        std::pair<int, int>(2, 7),
        std::pair<int, int>(2, 8),
    });


    x1 = 4;
    y1 = 7;
    x2 = 17;
    y2 = 15;
    pixel_width = 2;
    verify_line_manual(x1, y1, x2, y2, pixel_width, std::vector<std::pair<int, int>>{
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
        std::pair<int, int>(8, 7),
    });
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
