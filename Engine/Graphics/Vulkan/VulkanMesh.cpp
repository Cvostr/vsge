#include "VulkanMesh.hpp"

using namespace VSGE;

bool VulkanMesh::Create() {
	for (auto vbd : mVertexBuffers) {
		VulkanBuffer* vertexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
		vertexBuffer->Create(vbd.mVertexSize * mVerticesCount);
		vertexBuffer->WriteData(0, vbd.mVertexSize * mVerticesCount, vbd.mVertices);
		vertexBuffers.push_back(vertexBuffer);
	}
	if (mIndexCount > 0) {
		indexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_INDEX);
		indexBuffer->Create(sizeof(uint32) * mIndexCount);
		indexBuffer->WriteData(0, sizeof(uint32) * mIndexCount, mIndexArray);
	}

	mCreated = true;

	return true;
}

void VulkanMesh::Destroy() {
	if (mCreated) {
		for (auto vertbuffer : vertexBuffers) {
			vertbuffer->Destroy();
		}
		if(indexBuffer)
			indexBuffer->Destroy();

		mCreated = false;
	}
}