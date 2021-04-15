#pragma once

#define PI_FLOAT 3.14159265f

#define REAL_NUM_EQ(x, y) (fabs((x) - (y)) < 0.000001f)

float to_radians(float deg) {
	return deg * PI_FLOAT / 180.f;
}

template <typename T, typename U>
T lerp(T v1, T v2, U factor) {
    T result = v1 * (1.f - factor) + v2 * factor;
    return result;
}