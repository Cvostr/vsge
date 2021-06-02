#include "Mesh.hpp"

using namespace VSGE;

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	_verticesCount = vertexCount;

	_meshBoundingBox.CreateFromVertexArray(vertices, vertexCount);
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