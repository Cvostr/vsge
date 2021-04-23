#include "VulkanMesh.hpp"

using namespace VSGE;

void VulkanMesh::Create(Vertex* vertices, uint32 vertex_count) {
	mVerticesCount = vertex_count;

	vertexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
	vertexBuffer->Create(sizeof(Vertex) * vertex_count);
	vertexBuffer->WriteData(0, sizeof(Vertex) * vertex_count, vertices);

	mMeshBoundingBox.CreateFromVertexArray(vertices, vertex_count);

}
void VulkanMesh::Create(Vertex* vertices, uint32* indices, uint32 vertex_count, uint32 index_count) {
	mIndexCount = index_count;
	
	Create(vertices, vertex_count);

	indexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_INDEX);
	indexBuffer->Create(sizeof(uint32) * index_count);
	indexBuffer->WriteData(0, sizeof(uint32) * index_count, indices);
}
void VulkanMesh::Destroy() {
	//if (mCreated) {
	if(vertexBuffer)
		vertexBuffer->Destroy();
	if(indexBuffer)
		indexBuffer->Destroy();
	//}
}