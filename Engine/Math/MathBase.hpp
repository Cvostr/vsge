#pragma once

#define PI_FLOAT 3.14159265f

#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

#include <algorithm>

float to_radians(float deg);

template<typename T>
float clamp(T x, T minVal, T maxVal)
{
    return std::min(std::max(x, minVal), maxVal);
}

template<typename T>
T NormalizeAngle(T angle)
{
    while (angle > 360)
        angle -= 360;
    while (angle < 0)
        angle += 360;
    return angle;
}

template<typename T>
T max(T a1, T a2)
{
    if(a1 < a2)
        return a2;
    return a1;
}

template <typename T, typename U>
T lerp(T v1, T v2, U factor)
{
    T result = v1 * (1.f - factor) + v2 * factor;
    return result;
}

template<typename T>
T determinant3x3(T a, T b, T c, T d, T e, T f,
    T g, T h, T i) 
{
    //a b c
    //d e f
    //g h i
    return (a * e * i) +
        (d * c * h) +
        (g * b * f) -
        (g * e * c) -
        (d * b * i) -
        (h * f * a);
}