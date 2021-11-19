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

	float Dot(const tVec2& v) {
		return x * v.x + y * v.y;
	}

	tVec2 operator+(const tVec2& r) const {
		return tVec2(x + r.x, y + r.y);
	}

	tVec2 operator-(const tVec2& r) const {
		return tVec2(x - r.x, y - r.y);
	}

	tVec2 operator*(T d) const {
		return tVec2(x * d, y * d);
	}

	tVec2 operator/(T d) const {
		return tVec2(x / d, y / d);
	}

	tVec2 operator*(const tVec2& v) const {
		return tVec2(x * v.x, y * v.y);
	}

	void operator*=(const tVec2& v) {
		x *= v.x;
		y *= v.y;
	}

	void operator*=(T v) {
		x *= v;
		y *= v;
	}

	void operator/=(T v) {
		x /= v;
		y /= v;
	}

	bool operator ==(const tVec2& r) const {
		return x == r.x && y == r.y;
	}

	void operator+=(const tVec2& v) {
		x += v.x;
		y += v.y;
	}

};

typedef tVec2<float> Vec2;
typedef tVec2<int> Vec2i;