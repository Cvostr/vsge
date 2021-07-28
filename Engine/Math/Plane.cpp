#include "Plane.hpp"

using namespace VSGE;

float Plane::Distance(const Vec3& point) const { 
	return _normal.Dot(point) + _d; 
}
const Vec3& Plane::GetNormal() const { 
	return _normal; 
}
float Plane::GetD() const { 
	return _d; 
}

void Plane::SetNormal(const Vec3& normal) {
	_normal = normal;
}

void Plane::SetD(float d) {
	_d = d;
}

void Plane::Create(const Vec3& Normal, float D) {
	_normal = Normal.GetNormalized();
	_d = D;
}

void Plane::Create(const Vec3& Normal, const Vec3& Point) {
	_normal = Normal.GetNormalized();

	_d = (_normal * -1).Dot(Point);
}
void Plane::Create(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
	Vec3 v1v2 = v2 - v1;
	Vec3 v1v3 = v3 - v1;

	Vec3 Normal = v1.Cross(v2);
	Create(Normal, v3);
}

void Plane::Normalize() {
	float mag = sqrt(_normal.x * _normal.x + _normal.y * _normal.y + _normal.z * _normal.z);
	_normal.x = _normal.x / mag;
	_normal.y = _normal.y / mag;
	_normal.z = _normal.z / mag;
	_d = _d / mag;
}

Mat4 Plane::GetReflectionMatrix() const {
	Mat4 out = Mat4(1);

	float A = _normal.x;
	float B = _normal.y;
	float C = _normal.z;

	out.Values[0][0] = 1.f - 2.f * A * A;
	out.Values[0][1] = -2.f * A * B;
	out.Values[0][2] = -2.f * A * C;
	out.Values[0][3] = -2.f * A * _d;

	out.Values[1][0] = -2.f * A * B;
	out.Values[1][1] = 1.f - 2.f * B * B;
	out.Values[1][2] = -2.f * B * C;
	out.Values[1][3] = -2.f * B * _d;

	out.Values[2][0] = -2.f * A * C;
	out.Values[2][1] = -2.f * B * C;
	out.Values[2][2] = 1.f - 2.f * C * C;
	out.Values[2][3] = -2.f * C * _d;

	return out;
}