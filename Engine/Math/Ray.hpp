#pragma once

#include "Vec3.hpp"

namespace VSGE {

	class Ray {
	private:
		Vec3 _origin;
		Vec3 _direction;
	public:
		Ray() {

		}

		Ray(const Vec3& origin, const Vec3& direction) :
			_origin(origin),
			_direction(direction)
		{}

		const Vec3& GetOrigin() {
			return _origin;
		}

		const Vec3& GetDirection() {
			return _direction;
		}

		Vec3 GetPoint(float dist);
	};

	class RayHit {
	private:
	private:
		float _distance;
		Vec3 _hitPoint;

	public:
	};
}