#include "Ray.hpp"

using namespace VSGE;

Vec3 Ray::GetPoint(float dist) {
	return _origin + _direction * dist;
}