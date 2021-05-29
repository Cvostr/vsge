#pragma once

#include <cmath>
#include "Vec3.hpp"
#include "MathBase.hpp"

class Quat {
public:
	float x;
	float y;
	float z;
	float w;

	Quat() : 
		x(0), y(0), z(0), w(1)
	{}

	Quat(float x, float y, float z, float w) :
		x(x), y(y), z(z), w(w)
	{}

	Quat MultiplyTo(const Quat& qb);

	Quat Conjugate() const;

    float Length() const;

	float LengthSquared() const;

	Quat GetNormalized() const;

	Quat Inverse() const;

	Quat slerp(Quat& s, float factor);

    Vec3 GetEulerAngles() const;

	void CreateFromYawPitchRoll(float yaw, float pitch, float roll);

	void CreateFromEulerAngles(const Vec3& euler);

	Quat operator *(float rhs);

	Quat& operator *=(float rhs);

	Quat operator*(const Quat& q) {
		return MultiplyTo(q);
	}

	void operator*=(const Quat& q) {
		*this = MultiplyTo(q);
	}

	float GetRoll() const {
		float _y = 2.f * (x * y + w * z);
		float _x = w * w + x * x - y * y - z * z;
		return std::atan2(_y, _x);
	}

	float GetPitch() const {
		float _y = 2.f * (y * z + w * x);
		float _x = w * w - x * x - y * y + z * z;
		return std::atan2(_y, _x);
	}

	float GetYaw() const {

		return asin(clamp(-2.f * (x * z - w * y), -1.f, 1.f));
	}
};