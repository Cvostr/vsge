#include "Mesh.hpp"

using namespace VSGE;

Mesh::Mesh() :
	m_indexArray(nullptr),
	_positions_array(nullptr),
	m_verticesCount(0),
	m_indicesCount(0)
{}

Mesh::~Mesh() {
	SAFE_RELEASE_ARR(_positions_array);
	SAFE_RELEASE_ARR(m_indexArray);
}

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	m_verticesCount = vertexCount;
	//Create aabb
	_meshBoundingBox.CreateFromVertexArray(vertices, vertexCount);
	//store positions on separate array
	SAFE_RELEASE_ARR(_positions_array);
	_positions_array = new Vec3[vertexCount];
	for (uint32 vertex_i = 0; vertex_i < vertexCount; vertex_i++) {
		_positions_array[vertex_i] = vertices[vertex_i].pos;
	}
}

void Mesh::SetIndexBuffer(uint32* indices, uint32 indicesCount) {
	SAFE_RELEASE_ARR(m_indexArray)
	m_indexArray = new uint32[indicesCount];
	for (uint32 index_i = 0; index_i < indicesCount; index_i++) {
		m_indexArray[index_i] = indices[index_i];
	}
	m_indicesCount = indicesCount;
}

void Mesh::SetBones(Bone* bones, uint32 size) {
	m_bones.clear();
	for (uint32 i = 0; i < size; i++) {
		m_bones.push_back(bones[i]);
	}
}

uint32 Mesh::GetTrianglesCount() {
	if (m_indicesCount > 0)
		return m_indicesCount / 3;
	return m_verticesCount / 3;
}

uint32 Mesh::GetVerticesCount() const 
{ 
	return m_verticesCount; 
}

uint32 Mesh::GetIndexCount() const
{ 
	return m_indicesCount; 
}

const AABB& Mesh::GetBoundingBox() const
{ 
	return _meshBoundingBox; 
}

const tBonesList& Mesh::GetBones() const
{
	return m_bones;
}

void Mesh::GetTriangle(uint32 triangle_index, Vec3& v0, Vec3& v1, Vec3& v2) {
	if (m_indicesCount > 0) {
		v0 = _positions_array[m_indexArray[triangle_index * 3]];
		v1 = _positions_array[m_indexArray[triangle_index * 3 + 1]];
		v2 = _positions_array[m_indexArray[triangle_index * 3 + 2]];
	}
	else {
		v0 = _positions_array[triangle_index * 3];
		v1 = _positions_array[triangle_index * 3 + 1];
		v2 = _positions_array[triangle_index * 3 + 2];
	}
}