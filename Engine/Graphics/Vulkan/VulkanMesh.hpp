#pragma once

#include "../Mesh.hpp"
#include "VulkanBuffer.hpp"

namespace VSGE {
	class VulkanMesh : public Mesh {
	private:
		std::vector<VulkanBuffer*> vertexBuffers;
		VulkanBuffer* indexBuffer;
	public:

		VulkanMesh() :
			indexBuffer(nullptr) {}

		~VulkanMesh() {
			Destroy();
		}

		VulkanBuffer** GetVertexBuffers() {
			return vertexBuffers.data();
		}

		VulkanBuffer* GetIndexBuffer() {
			return indexBuffer;
		}

		VkBuffer GetVertexBufferVk(uint32 i = 0) {
			return vertexBuffers[i]->GetBuffer();
		}

		VkBuffer GetIndexBufferVk() {
			return indexBuffer->GetBuffer();
		}

		bool Create();
		void Destroy();
	};
}