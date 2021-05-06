#include "Mesh.hpp"

using namespace VSGE;

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	mVerticesCount = vertexCount;

	mMeshBoundingBox.CreateFromVertexArray(vertices, vertexCount);
}

void Mesh::SetIndexBuffer(uint32* indices, uint32 indicesCount) {
	mIndexArray = indices;
	mIndexCount = indicesCount;
}