#include "Mesh.hpp"

using namespace VSGE;

Mesh::Mesh() :
	_indexArray(nullptr),
	_positions_array(nullptr),
	_verticesCount(0),
	_indicesCount(0)
{}

Mesh::~Mesh() {
	SAFE_RELEASE_ARR(_positions_array);
	SAFE_RELEASE_ARR(_indexArray);
}

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	_verticesCount = vertexCount;
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
	SAFE_RELEASE_ARR(_indexArray)
	_indexArray = new uint32[indicesCount];
	for (uint32 index_i = 0; index_i < indicesCount; index_i++) {
		_indexArray[index_i] = indices[index_i];
	}
	_indicesCount = indicesCount;
}

void Mesh::SetBones(Bone* bones, uint32 size) {
	for (uint32 i = 0; i < size; i++) {
		_bones.push_back(bones[i]);
	}
}

uint32 Mesh::GetTrianglesCount() {
	if (_indicesCount > 0)
		return _indicesCount / 3;
	return _verticesCount / 3;
}

void Mesh::GetTriangle(uint32 triangle_index, Vec3& v0, Vec3& v1, Vec3& v2) {
	if (_indicesCount > 0) {
		v0 = _positions_array[_indexArray[triangle_index * 3]];
		v1 = _positions_array[_indexArray[triangle_index * 3 + 1]];
		v2 = _positions_array[_indexArray[triangle_index * 3 + 2]];
	}
	else {
		v0 = _positions_array[triangle_index * 3];
		v1 = _positions_array[triangle_index * 3 + 1];
		v2 = _positions_array[triangle_index * 3 + 2];
	}
}