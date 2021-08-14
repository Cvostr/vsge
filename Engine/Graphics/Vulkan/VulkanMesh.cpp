#include "VulkanMesh.hpp"

using namespace VSGE;

bool VulkanMesh::Create() {
	for (auto vbd : _vertexBuffers) {
		VulkanBuffer* vertexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
		vertexBuffer->Create(vbd.mVertexSize * _verticesCount);
		vertexBuffer->WriteData(0, vbd.mVertexSize * _verticesCount, vbd.mVertices);
		vertexBuffers.push_back(vertexBuffer);
	}
	if (_indicesCount > 0) {
		indexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_INDEX);
		indexBuffer->Create(sizeof(uint32) * _indicesCount);
		indexBuffer->WriteData(0, sizeof(uint32) * _indicesCount, _indexArray);
	}

	mCreated = true;

	return true;
}

void VulkanMesh::Destroy() {
	if (mCreated) {
		for (auto vertbuffer : vertexBuffers) {
			vertbuffer->Destroy();
			SAFE_RELEASE(vertbuffer);
		}
		vertexBuffers.clear();
		_vertexBuffers.clear();

		if (indexBuffer) {
			indexBuffer->Destroy();
			SAFE_RELEASE(indexBuffer)
		}

		mCreated = false;
	}
}