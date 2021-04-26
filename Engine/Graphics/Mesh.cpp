#include "Mesh.hpp"

using namespace VSGE;

void Mesh::SetVertexBuffer(Vertex* vertices, uint32 vertexCount) {
	AddVertexBuffer(vertices);

	mVerticesCount = vertexCount;

	mMeshBoundingBox.CreateFromVertexArray(vertices, vertexCount);
}
template <typename T>
void Mesh::AddVertexBuffer(T* vertices) {
	VertexBufferData vbd;
	vbd.mVertices = (byte*)vertices;
	vbd.mVertexSize = sizeof(T);
	mVertexBuffers.push_back(vbd);
}
void Mesh::SetIndexBuffer(uint32* indices, uint32 indicesCount) {
	mIndexArray = indices;
	mIndexCount = indicesCount;
}