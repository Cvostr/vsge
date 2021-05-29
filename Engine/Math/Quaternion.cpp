#include "Quaternion.hpp"

Quat Quat::Conjugate() const {
    return Quat(-x, -y, -z, w); 
}

float Quat::Length() const { //Calculates module length of vector
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Quat::LengthSquared() const 
{ 
    return (x * x) + (y * y) + (z * z) + (w * w); 
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

Quat Quat::operator*(float rhs) {
    return Quat(x * rhs, y * rhs, z * rhs, w * rhs);
}

Vec3 Quat::GetEulerAngles() const {
    float RAD_TO_DEG = 180.f / 3.14159265f;
    Vec3 result;
    /*float sinr_cosp = 2 * (w * x + y * z);
    float cosr_cosp = 1 - 2 * (x * x + y * y);
    result.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        result.y = std::copysign(PI_FLOAT / 2, sinp); // use 90 degrees if out of range
    else
        result.y = std::asin(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (w * z + x * y);
    float cosy_cosp = 1 - 2 * (y * y + z * z);
    result.z = std::atan2(siny_cosp, cosy_cosp);*/

    result.y = GetYaw();
    result.x = GetPitch();
    result.z = GetRoll();

    return result * RAD_TO_DEG;
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

Quat Quat::Inverse() const
{
    float length_squared = LengthSquared();
    if (length_squared == 1.0f)
        return Conjugate();
    else if (length_squared >= std::numeric_limits<float>::epsilon())
        return Conjugate() * (1.0f / length_squared);
    else
        return Quat(0, 0, 0, 1);
}