#include "AABB.hpp"
#include "Vec4.hpp"
#include <Core/VarTypes/Base.hpp>
#include "Math/MatrixTransform.hpp"

AABB::AABB(const Vec3& min, const Vec3& max) {
	m_min = min;
	m_max = max;
}

AABB::AABB() {
	PrepareForExtend(); 
}

void AABB::SetZero() {
	m_min = Vec3(0);
	m_max = Vec3(0);
}

void AABB::Extend(const AABB& box) {
	if (box.m_min.x < m_min.x)
		m_min.x = box.m_min.x;
	if (box.m_min.y < m_min.y)
		m_min.y = box.m_min.y;
	if (box.m_min.z < m_min.z)
		m_min.z = box.m_min.z;

	if (box.m_max.x > m_max.x)
		m_max.x = box.m_max.x;
	if (box.m_max.y > m_max.y)
		m_max.y = box.m_max.y;
	if (box.m_max.z > m_max.z)
		m_max.z = box.m_max.z;
}

void AABB::Extend(const Vec3& vec) {
	if (m_min.x > vec.x) m_min.x = vec.x;
	if (m_min.y > vec.y) m_min.y = vec.y;
	if (m_min.z > vec.z) m_min.z = vec.z;
				 			 
	if (m_max.x < vec.x) m_max.x = vec.x;
	if (m_max.y < vec.y) m_max.y = vec.y;
	if (m_max.z < vec.z) m_max.z = vec.z;
}

void AABB::PrepareForExtend() {
	m_min = Vec3(MAX_FLOAT);
	m_max = Vec3(-MAX_FLOAT);
}

void AABB::CreateFromVertexArray(Vertex* array, unsigned int vertexCount) {
	PrepareForExtend();
	for (unsigned int v_i = 0; v_i < vertexCount; v_i++) {
		Extend(array[v_i].pos);
	}
}

bool AABB::IsIntersects(const AABB& box) const {
	if ((this->m_max.x < box.m_min.x) || (this->m_min.x > box.m_max.x) ||
		(this->m_max.y < box.m_min.y) || (this->m_min.y > box.m_max.y) ||
		(this->m_max.z < box.m_min.z) || (this->m_min.z > box.m_max.z))
	{
		return false;
	}
	return true;
}

bool AABB::IsInside(const Vec3& point) const {
	if (point.x < m_min.x || point.x > m_max.x ||
		point.y < m_min.y || point.y > m_max.y ||
		point.z < m_min.z || point.z > m_max.z)
	{
		return false;
	}

		return true;
}

void AABB::ApplyTransform(const Mat4& transform) {
	Vec3 corners[8];
	corners[0] = m_min;
	corners[1] = Vec3(m_min.x, m_max.y, m_min.z);
	corners[2] = Vec3(m_min.x, m_max.y, m_max.z);
	corners[3] = Vec3(m_min.x, m_min.y, m_max.z);
	corners[4] = Vec3(m_max.x, m_min.y, m_min.z);
	corners[5] = Vec3(m_max.x, m_max.y, m_min.z);
	corners[6] = m_max;
	corners[7] = Vec3(m_max.x, m_min.y, m_max.z);

	PrepareForExtend();
	for (int i = 0; i < 8; i++) {
		Vec4 v4 = Vec4(corners[i], 1.0f);
		v4 = transform * v4;
		Extend(v4.Vec3());
	}
}

Vec3 AABB::GetCenter() const {
	return (m_min + m_max) * 0.5f;
}

Vec3 AABB::GetSize() const {
	return (m_max - m_min) * 0.5;
}

Mat4 AABB::GetMatrix() const {
	return GetScaleMatrix(GetSize()) * GetTranslationMatrix(GetCenter());
}