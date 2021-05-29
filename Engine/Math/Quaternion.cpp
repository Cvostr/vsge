#include "Quaternion.hpp"

Quat Quat::Conjugate() const {
    return Quat(-x, -y, -z, w); 
}

float Quat::Length() const { //Calculates module length of vector
	return sqrtf(x * x + y * y + z * z + w * w);
}

Quat Quat::GetNormalized() const {
	float coeff = 1.f / Length();
	return Quat(x * coeff, y * coeff, z * coeff, w * coeff);
}

Quat Quat::MultiplyTo(const Quat& qb) {
   float num4 = qb.x;
   float num3 = qb.y;
   float num2 = qb.z;
   float num = qb.w;
   float num12 = (y * num2) - (z * num3);
   float num11 = (z * num4) - (x * num2);
   float num10 = (x * num3) - (y * num4);
   float num9 = ((x * num4) + (y * num3)) + (z * num2);

    return Quat(
        ((x * num) + (num4 * w)) + num12,
        ((y * num) + (num3 * w)) + num11,
        ((z * num) + (num2 * w)) + num10,
        (w * num) - num9
    );
}

Quat Quat::slerp(Quat& s, float factor) {
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

Quat& Quat::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;

    return *this;
}

Vec3 Quat::GetEulerAngles() const {
	const float check = 2.0f * (y * z + w * x);
	float RAD_TO_DEG = 180.f / 3.14159265f;

	if (check < -0.995f)
	{
		return Vec3
		(
			-90.0f,
			0.0f,
			-atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
		);
	}

	if (check > 0.995f)
	{
		return Vec3
		(
			90.0f,
			0.0f,
			atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
		);
	}

	return Vec3
	(
		asinf(check) * RAD_TO_DEG,
		atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y)) * RAD_TO_DEG,
		atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)) * RAD_TO_DEG
	);
}

void Quat::CreateFromYawPitchRoll(float yaw, float pitch, float roll) {
    const float halfRoll = roll * 0.5f;
    const float halfPitch = pitch * 0.5f;
    const float halfYaw = yaw * 0.5f;

    const float sinRoll = sin(halfRoll);
    const float cosRoll = cos(halfRoll);
    const float sinPitch = sin(halfPitch);
    const float cosPitch = cos(halfPitch);
    const float sinYaw = sin(halfYaw);
    const float cosYaw = cos(halfYaw);

    *this = Quat(
        cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
        sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
        cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
        cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
    );
}

void Quat::CreateFromEulerAngles(const Vec3& euler) {
    CreateFromYawPitchRoll(to_radians(euler.y), to_radians(euler.x), to_radians(euler.z));
}