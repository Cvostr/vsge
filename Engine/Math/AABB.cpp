#include "AABB.hpp"
#include "Vec4.hpp"
#include <Core/VarTypes/Base.hpp>
#include "Math/MatrixTransform.hpp"

void AABB::SetZero() {
	_min = Vec3(0);
	_max = Vec3(0);
}

void AABB::Extend(const AABB& box) {
	if (box._min.x < _min.x)
		_min.x = box._min.x;
	if (box._min.y < _min.y)
		_min.y = box._min.y;
	if (box._min.z < _min.z)
		_min.z = box._min.z;

	if (box._max.x > _max.x)
		_max.x = box._max.x;
	if (box._max.y > _max.y)
		_max.y = box._max.y;
	if (box._max.z > _max.z)
		_max.z = box._max.z;
}

void AABB::Extend(const Vec3& vec) {
	if (_min.x > vec.x) _min.x = vec.x;
	if (_min.y > vec.y) _min.y = vec.y;
	if (_min.z > vec.z) _min.z = vec.z;
				 			 
	if (_max.x < vec.x) _max.x = vec.x;
	if (_max.y < vec.y) _max.y = vec.y;
	if (_max.z < vec.z) _max.z = vec.z;
}

void AABB::PrepareForExtend() {
	_min = Vec3(MAX_FLOAT);
	_max = Vec3(-MAX_FLOAT);
}

void AABB::CreateFromVertexArray(Vertex* array, unsigned int vertexCount) {
	PrepareForExtend();
	for (unsigned int v_i = 0; v_i < vertexCount; v_i++) {
		Extend(array[v_i].pos);
	}
}

bool AABB::IsIntersects(const AABB& box) const {
	if ((this->_max.x < box._min.x) || (this->_min.x > box._max.x) ||
		(this->_max.y < box._min.y) || (this->_min.y > box._max.y) ||
		(this->_max.z < box._min.z) || (this->_min.z > box._max.z))
	{
		return false;
	}
	return true;
}

bool AABB::IsInside(const Vec3& point) const {
	if (point.x < _min.x || point.x > _max.x ||
		point.y < _min.y || point.y > _max.y ||
		point.z < _min.z || point.z > _max.z)
	{
		return false;
	}

		return true;
}

void AABB::ApplyTransform(const Mat4& transform) {
	Vec3 corners[8];
	corners[0] = _min;
	corners[1] = Vec3(_min.x, _max.y, _min.z);
	corners[2] = Vec3(_min.x, _max.y, _max.z);
	corners[3] = Vec3(_min.x, _min.y, _max.z);
	corners[4] = Vec3(_max.x, _min.y, _min.z);
	corners[5] = Vec3(_max.x, _max.y, _min.z);
	corners[6] = _max;
	corners[7] = Vec3(_max.x, _min.y, _max.z);

	PrepareForExtend();
	for (int i = 0; i < 8; i++) {
		Vec4 v4 = Vec4(corners[i], 1.0f);
		v4 = transform * v4;
		Extend(v4.Vec3());
	}
}

Vec3 AABB::GetCenter() const {
	return (_min + _max) * 0.5f;
}

Vec3 AABB::GetSize() const {
	return (_max - _min) * 0.5;
}

Mat4 AABB::GetMatrix() const {
	return GetScaleMatrix(GetSize()) * GetTranslationMatrix(GetCenter());
}