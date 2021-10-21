#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Vertex.hpp"

class AABB {
private:
	Vec3 _min;
	Vec3 _max;

public:
	
	void SetZero();
	void Extend(const AABB& box);
	void Extend(const Vec3& vec);
	void PrepareForExtend();
	bool IsIntersects(const AABB& box) const;
	bool IsInside(const Vec3& point) const;
	void CreateFromVertexArray(Vertex* array, unsigned int vertexCount);
	void ApplyTransform(const Mat4& transform);

	const Vec3& GetMin() const {
		return _min;
	}

	const Vec3& GetMax() const {
		return _max;
	}

	Vec3 GetCenter() const;
	Vec3 GetSize() const;
	Mat4 GetMatrix() const;

	AABB(const Vec3& min, const Vec3& max) {
		_min = min;
		_max = max;
	}
	AABB() { PrepareForExtend(); }
};