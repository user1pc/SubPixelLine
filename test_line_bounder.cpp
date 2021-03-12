
#include <stdlib.h>
#include <utility>
#include <vector>
#include <functional>
#include <cmath>
#include "gtest/gtest.h"
#include "line_bounder.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

bool test_line_bounder_correct(double x1, double y1, double x2, double y2,
    double x_min, double x_max, double y_min, double y_max,
    double *out_t1, double *out_t2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    double t1x = (((dx >= 0) ? x_min : x_max) - x1) / dx;
    double t1y = (((dy >= 0) ? y_min : y_max) - y1) / dy;
    double t2x = (((dx >= 0) ? x_max : x_min) - x1) / dx;
    double t2y = (((dy >= 0) ? y_max : y_min) - y1) / dy;
    double t_near, t_far;

    if (fabs(dx) < 0.00001)
    {
        t_near = t1y;
        t_far = t2y;
        *out_t1 = t_near;
        *out_t2 = t_far;
        *out_t1 = max(*out_t1, 0.0);
        *out_t2 = min(*out_t2, 1.0);
        return t_near < t_far && t_near < 1.0 && t_far > 0.0 && x1 >= x_min && x1 <= x_max;
    }
    else if (fabs(dy) < 0.00001)
    {
        t_near = t1x;
        t_far = t2x;
        *out_t1 = t_near;
        *out_t2 = t_far;
        *out_t1 = max(*out_t1, 0.0);
        *out_t2 = min(*out_t2, 1.0);
        return t_near < t_far && t_near < 1.0 && t_far > 0.0 && y1 >= y_min && y1 <= y_max;
    }
    else
    {
        t_near = max(t1x, t1y);
        t_far = min(t2x, t2y);
    }
    *out_t1 = t_near;
    *out_t2 = t_far;
    *out_t1 = max(*out_t1, 0.0);
    *out_t2 = min(*out_t2, 1.0);
    return t_near < t_far && t_near < 1.0 && t_far > 0.0;
}

bool test_line_bounder_verify(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max)
{
    int32_t test_x1, test_y1, test_x2, test_y2;
    if (x1 == 271 && y1 == 491 && x2 == 638 && y2 == 223)
    {
        int g = 0;
    }
    bool test_hit = line_bound_inside_rect(x1, y1, x2, y2, x_min, y_min, x_max, y_max,
        &test_x1, &test_y1, &test_x2, &test_y2);
    
    double t1, t2;
    bool correct_hit = test_line_bounder_correct(x1, y1, x2, y2, x_min, x_max, y_min, y_max, &t1, &t2);
    if (test_hit != correct_hit)
        return false;
    double dx = (double)(x2 - x1);
    double dy = (double)(y2 - y1);
    int32_t correct_x1 = (int32_t)(x1 + dx * t1);
    int32_t correct_y1 = (int32_t)(y1 + dy * t1);
    int32_t correct_x2 = (int32_t)(x1 + dx * t2);
    int32_t correct_y2 = (int32_t)(y1 + dy * t2);
    if (correct_hit)
    {
        if (abs(test_x1 - correct_x1) >= 3)
            return false;
        if (abs(test_y1 - correct_y1) >= 3)
            return false;
        if (abs(test_x2 - correct_x2) >= 3)
            return false;
        if (abs(test_y2 - correct_y2) >= 3)
            return false;
    }
    return true;
}

TEST(auto_tests, LineBounder)
{
    srand(0);
    for (int i = 0; i < 100000000; i++)
    {
        int32_t x1 = rand() % 1024;
        int32_t y1 = rand() % 1024;
        int32_t x2 = rand() % 1024;
        int32_t y2 = rand() % 1024;

        if (x1 == x2 && y1 == y2)
            continue;

        int32_t x_min = rand() % 1022;
        int32_t y_min = rand() % 1022;
        int32_t x_max = x_min + (rand() % (1024 - x_min)) + 1;
        int32_t y_max = y_min + (rand() % (1024 - y_min)) + 1;
        test_line_bounder_verify(x1, y1, x2, y2, x_min, x_max, y_min, y_max);
    }
}

TEST(manual_tests, LineBounder)
{
    int32_t x1, y1, x2, y2;
    bool hit;
    
    hit = line_bound_inside_rect(2, 0, 2, 100, 0, 0, 4, 4, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 0);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 4);

    hit = line_bound_inside_rect(2, 0, 2, 100, 0, 1, 4, 4, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 4);

    hit = line_bound_inside_rect(2, 0, 2, 100, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(2, 0, 2, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(2, 2, 2, 100, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(2, 2, 2, 7, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 2);
    EXPECT_EQ(y2, 7);
    
    hit = line_bound_inside_rect(3, 20, 3, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(3, 8, 3, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 8);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(3, 20, 3, 2, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 2);

    hit = line_bound_inside_rect(3, 8, 3, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 8);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(3, 0, 75, 185, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 6);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(7, 5, 3, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 7);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(3, 0, 7, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 7);
    EXPECT_EQ(y2, 5);
    
    hit = line_bound_inside_rect(9, 9, 0, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 9);
    EXPECT_EQ(y1, 9);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(0, 5, 15, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(15, 5, 0, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(7, 5, 0, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 7);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(15, 5, 3, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 3);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(15, 7, 0, 3, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 5);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 3);

    hit = line_bound_inside_rect(0, 0, 11, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(11, 11, 0, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(3, 3, 11, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 3);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(0, 0, 5, 5, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 5);
    EXPECT_EQ(y2, 5);

    hit = line_bound_inside_rect(3, 3, 6, 6, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 3);
    EXPECT_EQ(x2, 6);
    EXPECT_EQ(y2, 6);

    hit = line_bound_inside_rect(1, 1, 10, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(10, 10, 1, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(1, 10, 10, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(10, 1, 1, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(0, 11, 11, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(3, 8, 11, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 3);
    EXPECT_EQ(y1, 8);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(0, 11, 9, 2, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x2, 9);
    EXPECT_EQ(y2, 2);

    hit = line_bound_inside_rect(5, 6, 9, 2, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 5);
    EXPECT_EQ(y1, 6);
    EXPECT_EQ(x2, 9);
    EXPECT_EQ(y2, 2);


    hit = line_bound_inside_rect(1, 0, 1, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(1, 2, 1, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(1, 0, 1, 9, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 9);

    hit = line_bound_inside_rect(1, 9, 1, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 9);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(1, 2, 1, 9, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 9);

    hit = line_bound_inside_rect(1, 9, 1, 2, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y1, 9);
    EXPECT_EQ(x2, 1);
    EXPECT_EQ(y2, 2);


    hit = line_bound_inside_rect(10, 0, 10, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(10, 2, 10, 11, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 10);

    hit = line_bound_inside_rect(10, 0, 10, 9, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 9);

    hit = line_bound_inside_rect(10, 9, 10, 0, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 9);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 1);

    hit = line_bound_inside_rect(10, 2, 10, 9, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 2);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 9);

    hit = line_bound_inside_rect(10, 9, 10, 2, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(x1, 10);
    EXPECT_EQ(y1, 9);
    EXPECT_EQ(x2, 10);
    EXPECT_EQ(y2, 2);
    




    hit = line_bound_inside_rect(0, 1, 11, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 10);

    hit = line_bound_inside_rect(2, 1, 11, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 10);

    hit = line_bound_inside_rect(0, 1, 9, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 9);

    hit = line_bound_inside_rect(9, 1, 0, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 9);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 1);
    
    hit = line_bound_inside_rect(2, 1, 9, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 9);

    hit = line_bound_inside_rect(9, 1, 2, 1, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 1);
    EXPECT_EQ(x1, 9);
    EXPECT_EQ(y2, 1);
    EXPECT_EQ(x2, 2);


    hit = line_bound_inside_rect(0, 10, 11, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 10);

    hit = line_bound_inside_rect(2, 10, 11, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 10);

    hit = line_bound_inside_rect(0, 10, 9, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 1);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 9);

    hit = line_bound_inside_rect(9, 10, 0, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 9);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 1);

    hit = line_bound_inside_rect(2, 10, 9, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 2);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 9);

    hit = line_bound_inside_rect(9, 10, 2, 10, 1, 1, 10, 10, &x1, &y1, &x2, &y2);
    EXPECT_TRUE(hit);
    EXPECT_EQ(y1, 10);
    EXPECT_EQ(x1, 9);
    EXPECT_EQ(y2, 10);
    EXPECT_EQ(x2, 2);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
