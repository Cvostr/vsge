#pragma once

#include "Vec2.hpp"
#include <cmath>

	template<typename T>
	class tVec3 {
	public:
		T x;
		T y;
		T z;

		tVec3() : x(0), y(0), z(0) {}
		tVec3(T v) : x(v), y(v), z(v) {}
		tVec3(T x, T y, T z) : x(x), y(y), z(z) {}
		tVec3(const tVec2<T> xy, float z) : x(xy.x), y(xy.y), z(z) {}

		tVec3 GetNormalized() const {
			float coeff = 1.f / Length();
			return tVec3(x * coeff, y * coeff, z * coeff);
		}

		void Normalize() {
			*this = GetNormalized();
		}

		tVec3 Invert() const {
			return tVec3(1.f / x, 1.f / y, 1.f / z);
		}

		float Length() const { //Calculates module length of vector
			return sqrtf(x * x + y * y + z * z);
		}

		float DistanceTo(const tVec3& v) const {
			return (*this - v).Length();
		}

		float Dot(const tVec3& v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		tVec3 Cross(const tVec3& v) const {
			return tVec3(y * v.z - z * v.y,
						z * v.x - x * v.z,
						x * v.y - y * v.x);
		}

		tVec3 operator+(const tVec3& r) const {
			return tVec3(x + r.x, y + r.y, z + r.z);
		}

		tVec3 operator-(const tVec3& r) const {
			return tVec3(x - r.x, y - r.y, z - r.z);
		}

		tVec3 operator*(T d) const {
			return tVec3(x * d, y * d, z * d);
		}

		tVec3 operator*(const tVec3& v) const {
			return tVec3(x * v.x, y * v.y, z * v.z);
		}

		tVec3 operator/(T d) const {
			return tVec3(x / d, y / d, z / d);
		}

		tVec3 operator/(const tVec3& v) const {
			return tVec3(x / v.x, y / v.y, z / v.z);
		}

		void operator=(const tVec3& v) {
			x = v.x;
			y = v.y;
			z = v.z;
		}

		void operator+=(const tVec3& v) {
			x += v.x;
			y += v.y;
			z += v.z;
		}

		void operator*=(const tVec3& v) {
			x *= v.x;
			y *= v.y;
			z *= v.z;
		}

		void operator*=(T v) {
			x *= v;
			y *= v;
			z *= v;
		}

		void operator/=(T v) {
			x /= v;
			y /= v;
			z /= v;
		}

		bool operator ==(const tVec3& r) const {
			return x == r.x && y == r.y && z == r.z;
		}

		bool operator !=(const tVec3& r) const {
			return x != r.x && y != r.y && z != r.z;
		}
	};

	typedef tVec3<float> Vec3;
	typedef tVec3<int> Vec3i;