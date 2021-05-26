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