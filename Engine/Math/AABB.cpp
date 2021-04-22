#include "AABB.hpp"
#include "Vec4.hpp"
#include <Core/VarTypes/Base.hpp>
#include "Math/MatrixTransform.hpp"

void AABB::Extend(const AABB& box) {
	if (box.mMin.x < mMin.x)
		mMin.x = box.mMin.x;
	if (box.mMin.y < mMin.y)
		mMin.y = box.mMin.y;
	if (box.mMin.z < mMin.z)
		mMin.z = box.mMin.z;

	if (box.mMax.x > mMax.x)
		mMax.x = box.mMax.x;
	if (box.mMax.y > mMax.y)
		mMax.y = box.mMax.y;
	if (box.mMax.z > mMax.z)
		mMax.z = box.mMax.z;
}

void AABB::Extend(const Vec3& vec) {
	if (mMin.x > vec.x) mMin.x = vec.x;
	if (mMin.y > vec.y) mMin.y = vec.y;
	if (mMin.z > vec.z) mMin.z = vec.z;
				 			 
	if (mMax.x < vec.x) mMax.x = vec.x;
	if (mMax.y < vec.y) mMax.y = vec.y;
	if (mMax.z < vec.z) mMax.z = vec.z;
}

void AABB::PrepareForExtend() {
	mMin = Vec3(MAX_FLOAT);
	mMax = Vec3(-MAX_FLOAT);
}

void AABB::CreateFromVertexArray(Vertex* array, unsigned int vertexCount) {
	PrepareForExtend();
	for (unsigned int v_i = 0; v_i < vertexCount; v_i++) {
		Extend(array[v_i].pos);
	}
}

bool AABB::IsIntersects(const AABB& box) {
	if ((this->mMax.x < box.mMin.x) || (this->mMin.x > box.mMax.x) ||
		(this->mMax.y < box.mMin.y) || (this->mMin.y > box.mMax.y) ||
		(this->mMax.z < box.mMin.z) || (this->mMin.z > box.mMax.z))
	{
		return false;
	}
	return true;
}

void AABB::ApplyTransform(const Mat4& transform) {
	Vec3 corners[8];
	corners[0] = mMin;
	corners[1] = Vec3(mMin.x, mMax.y, mMin.z);
	corners[2] = Vec3(mMin.x, mMax.y, mMax.z);
	corners[3] = Vec3(mMin.x, mMin.y, mMax.z);
	corners[4] = Vec3(mMax.x, mMin.y, mMin.z);
	corners[5] = Vec3(mMax.x, mMax.y, mMin.z);
	corners[6] = mMax;
	corners[7] = Vec3(mMax.x, mMin.y, mMax.z);

	PrepareForExtend();
	for (int i = 0; i < 8; i++) {
		Vec4 v4 = Vec4(corners[i], 1.0f);
		v4 = transform * v4;
		Extend(v4.Vec3());
	}
}

Vec3 AABB::GetCenter() {
	return (mMin + mMax) * 0.5f;
}

Vec3 AABB::GetSize() {
	return (mMax - mMin) * 0.5;
}

Mat4 AABB::GetMatrix() {
	return GetScaleMatrix(GetSize()) * GetTranslationMatrix(GetCenter());
}