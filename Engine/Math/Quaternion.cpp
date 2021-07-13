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

static float NormalizeAngle(float angle)
{
    while (angle > 360)
        angle -= 360;
    while (angle < 0)
        angle += 360;
    return angle;
}

static Vec3 NormalizeAngles(Vec3 angles)
{
    angles.x = NormalizeAngle(angles.x);
    angles.y = NormalizeAngle(angles.y);
    angles.z = NormalizeAngle(angles.z);
    return angles;
}

Quat Quat::MultiplyTo(const Quat& qb) const {
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

Quat Quat::operator*(const Quat& q) const {
    return MultiplyTo(q);
}

void Quat::operator*=(const Quat& q) {
    *this = MultiplyTo(q);
}

Vec3 Quat::GetEulerAngles() const {
    float RAD_TO_DEG = 180.f / 3.14159265f;

    float sqw = w * w;
    float sqx = x * x;
    float sqy = y * y;
    float sqz = z * z;
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    float test = x * w - y * z;
    Vec3 v;

    if (test > 0.4995f * unit) { // singularity at north pole
        v.y = 2.f * std::atan2f(y, x);
        v.x = PI_FLOAT / 2;
        v.z = 0;
        return NormalizeAngles(v * RAD_TO_DEG);
    }
    if (test < -0.4995f * unit) { // singularity at south pole
        v.y = -2.f * std::atan2f(y, x);
        v.x = -PI_FLOAT / 2;
        v.z = 0;
        return NormalizeAngles(v * RAD_TO_DEG);
    }

    Quat q = Quat(w, z, x, y);
    v.y = std::atan2f(2.f * q.x * q.w + 2.f * q.y * q.z, 1 - 2.f * (q.z * q.z + q.w * q.w));     // Yaw
    v.x = std::asin(2.f * (q.x * q.z - q.w * q.y));                             // Pitch
    v.z = std::atan2f(2.f * q.x * q.y + 2.f * q.z * q.w, 1 - 2.f * (q.y * q.y + q.z * q.z));      // Roll

    return NormalizeAngles(v * RAD_TO_DEG);
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