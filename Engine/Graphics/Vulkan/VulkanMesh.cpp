#include "VulkanMesh.hpp"
#include <Engine/Application.hpp>

using namespace VSGE;

VulkanMesh::VulkanMesh() :
	indexBuffer(nullptr) {}

VulkanMesh::~VulkanMesh() {
	Destroy();
}

bool VulkanMesh::Create() {
	if (!Application::Get()->GetDescription().headless) {
		for (auto vbd : _vertexBuffers) {
			VulkanBuffer* vertexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_VERTEX);
			vertexBuffer->Create(vbd.mVertexSize * _verticesCount, LOCATION_GPU);
			vertexBuffer->WriteData(0, vbd.mVertexSize * _verticesCount, vbd.mVertices);
			vertexBuffers.push_back(vertexBuffer);
		}
		if (_indicesCount > 0) {
			indexBuffer = new VulkanBuffer(GpuBufferType::GPU_BUFFER_TYPE_INDEX);
			indexBuffer->Create(sizeof(uint32) * _indicesCount, LOCATION_GPU);
			indexBuffer->WriteData(0, sizeof(uint32) * _indicesCount, _indexArray);
		}
	}

	mCreated = true;

	return true;
}

void VulkanMesh::Destroy() {
	SAFE_RELEASE_ARR(_positions_array);
	SAFE_RELEASE_ARR(_indexArray)
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