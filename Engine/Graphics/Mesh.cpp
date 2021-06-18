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
}

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	_verticesCount = vertexCount;
	//Create aabb
	_meshBoundingBox.CreateFromVertexArray(vertices, vertexCount);
	//store positions on separate array
	_positions_array = new Vec3[vertexCount];
	for (uint32 vertex_i = 0; vertex_i < vertexCount; vertex_i++) {
		_positions_array[vertex_i] = vertices[vertex_i].pos;
	}
}

void Mesh::SetIndexBuffer(uint32* indices, uint32 indicesCount) {
	_indexArray = indices;
	_indicesCount = indicesCount;
}

void Mesh::SetBones(Bone* bones, uint32 size) {
	for (uint32 i = 0; i < size; i++) {
		_bones.push_back(bones[i]);
	}
}