#pragma once

#include <cmath>
#include "Vec3.hpp"

template<typename T>
class tVec4 {
public:

	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
		T Values[4];
	};

	tVec4() : x(0), y(0), z(0), w(0) {}
	tVec4(T v) : x(v), y(v), z(v), w(v) {}
	tVec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	tVec4(tVec3<T> xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

	tVec4 GetNormalized() const {
		float coeff = 1.f / Length();
		return tVec4(x * coeff, y * coeff, z * coeff, w * coeff);
	}

	float Length() const { //Calculates module length of vector
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	const T& operator[](uint32 i) const {
		return Values[i];
	}

	T& operator[](uint32 i) {
		return Values[i];
	}

	tVec4 operator+(const tVec4& r) const {
		return tVec4(x + r.x, y + r.y, z + r.z, w + r.w);
	}

	tVec4 operator-(const tVec4& r) const {
		return tVec4(x - r.x, y - r.y, z - r.z, w - r.w);
	}

	tVec4 operator*(T d) const {
		return tVec4(x * d, y * d, z * d, w * d);
	}

	tVec4 operator/(T d) const {
		return tVec4(x / d, y / d, z / d, w / d);
	}

	bool operator ==(const tVec4& r) const {
		return x == r.x && y == r.y && z == r.z && w == r.w;
	}

	tVec3<T> Vec3() {
		return tVec3<T>(x, y, z);
	}
};

typedef tVec4<float> Vec4;
typedef tVec4<int> Vec4i;