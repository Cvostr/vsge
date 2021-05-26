#pragma once

#include "Plane.hpp"
#include <vector>

namespace VSGE {

	enum Side
	{
		LEFT = 0,
		RIGHT = 1,
		TOP = 2,
		BOTTOM = 3,
		BACK = 4,
		FRONT = 5
	};

	class Frustum {
	private:
		std::vector<Plane> _planes;
	public:

		void Update(const Mat4& PV);

		bool SphereInside(const Vec3& center, float radius);
	};
}