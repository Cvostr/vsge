#pragma once

#include <cmath>

template<typename T>
class tVec2 {
public:
	T x;
	T y;

	tVec2() : x(0), y(0) {}
	tVec2(T v) : x(v), y(v) {}
	tVec2(T x, T y) : x(x), y(y) {}

	tVec2 GetNormalized() {
		float coeff = 1.f / Length();
		return tVec2(x * coeff, y * coeff);
	}

	float Length() { //Calculates module length of vector
		return sqrtf(x * x + y * y);
	}

	tVec2 operator+(const tVec2& r) const {
		return tVec2(x + r.x, y + r.y);
	}

	tVec2 operator*(float d) const {
		return tVec2(x * d, y * d);
	}

	void operator*=(float v) {
		x *= v;
		y *= v;
	}

	bool operator ==(const tVec2& r) const {
		return x == r.x && y == r.y;
	}

};

typedef tVec2<float> Vec2;
typedef tVec2<int> Vec2i;