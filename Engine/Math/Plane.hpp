#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"

namespace VSGE {
	class Plane {
	private:
		Vec3 _normal;
		float _d;
	public:
		Plane() :
			_d(0)
		{
		}

		Plane(const Vec3 Normal, float D) :
			_normal(Normal),
			_d(D)
		{
		}

		Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
			//Create(v1, v2, v3);
		}

		Plane(const Vec3& Normal, const Vec3& Point) {
			//Create(Normal, Point);
		}

		float Distance(const Vec3& point) const { return vDot(mNormal, point) + mD; }

		const Vec3& GetNormal() { return mNormal; }
		float GetD() { return mD; }

		void Normalize();

		Mat4 GetReflectionMatrix();

		void Create(const Vec3& Normal, const Vec3& Point);
		void Create(const Vec3& v1, const Vec3& v2, const Vec3& v3);
		void Create(const Vec3& Normal, float D);
	};
}