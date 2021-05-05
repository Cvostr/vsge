#pragma once

#define PI_FLOAT 3.14159265f

#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

#include <algorithm>

float to_radians(float deg);

template<typename T>
float clamp(T x, T minVal, T maxVal) {
    return std::min(std::max(x, minVal), maxVal);
}


template <typename T, typename U>
T lerp(T v1, T v2, U factor) {
    T result = v1 * (1.f - factor) + v2 * factor;
    return result;
}