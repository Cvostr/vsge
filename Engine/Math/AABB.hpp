#pragma once

#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Vertex.hpp"

class AABB {
private:
	Vec3 mMin;
	Vec3 mMax;

public:
	
	void Extend(const AABB& box);
	void Extend(const Vec3& vec);
	void PrepareForExtend();
	bool IsIntersects(const AABB& box);
	void CreateFromVertexArray(Vertex* array, unsigned int vertexCount);
	void ApplyTransform(const Mat4& transform);

	Vec3 GetCenter();
	Vec3 GetSize();
	Mat4 GetMatrix();

	AABB(const Vec3& min, const Vec3& max) {
		mMin = min;
		mMax = max;
	}
	AABB() { PrepareForExtend(); }
};