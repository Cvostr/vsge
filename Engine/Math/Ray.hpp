#pragma once

#include "Vec3.hpp"
#include "AABB.hpp"

namespace VSGE {

	class Ray {
	private:
		Vec3 _origin; //start point
		Vec3 _direction; //direction vector
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

		float GetHitdistance(const AABB& box);
		bool IntersectTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, float &distance, Vec2& position) const;
	};

	class RayHit {
	private:
	private:
		float _distance;
		Vec3 _hitPoint;
		void* _hit;
	public:
		RayHit():
			_distance(0),
			_hit(nullptr)
		{}

		RayHit(float dist, const Vec3& hitPoint, void* hit = nullptr):
			_distance(dist),
			_hitPoint(hitPoint),
			_hit(hit)
		{}

		float GetDistance() const {
			return _distance;
		}

		const Vec3& GetHitPoint() {
			return _hitPoint;
		}

		void* GetHitObject() {
			return _hit;
		}
	};
}