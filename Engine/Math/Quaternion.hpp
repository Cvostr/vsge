#pragma once

#include <cmath>
#include "MathBase.hpp"

class Quat {
public:
	float x;
	float y;
	float z;
	float w;

	Quat() : 
		x(0), y(0), z(0), w(0)
	{}

	Quat(float x, float y, float z, float w) :
		x(x), y(y), z(z), w(w)
	{}

	float Length() const { //Calculates module length of vector
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	Quat GetNormalized() const {
		float coeff = 1.f / Length();
		return Quat(x * coeff, y * coeff, z * coeff, w * coeff);
	}

	Quat slerp(Quat& s, float factor) {
        Quat result;
        //Normalize quaternions
        Quat q1 = GetNormalized();
        Quat q2 = s.GetNormalized();

        float dot_product = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        float one_minus_blend = 1.0f - factor;

        if (dot_product < 0.0f)
        {
            result.x = q1.x * one_minus_blend + factor * -q2.x;
            result.y = q1.y * one_minus_blend + factor * -q2.y;
            result.z = q1.z * one_minus_blend + factor * -q2.z;
            result.w = q1.w * one_minus_blend + factor * -q2.w;
        }
        else
        {
            result.x = q1.x * one_minus_blend + factor * q2.x;
            result.y = q1.y * one_minus_blend + factor * q2.y;
            result.z = q1.z * one_minus_blend + factor * q2.z;
            result.w = q1.w * one_minus_blend + factor * q2.w;
        }

        return result.GetNormalized();
	}

    float GetRoll() {
        float _y = 2.f * (x * y + w * z);
        float _x = w * w + x * x - y * y - z * z;
        return std::atan2(_y, _x);
    }

    float GetPitch() {
        float _y = 2.f * (y * z + w * x);
        float _x = w * w - x * x - y * y + z * z;
        return std::atan2(_y, _x);
    }

    float GetYaw() {
        
        return asin(clamp(-2.f * (x * z - w * y), -1.f, 1.f));
    }
};