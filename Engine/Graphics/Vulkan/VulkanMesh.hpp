#pragma once

#include "../Mesh.hpp"
#include "VulkanBuffer.hpp"

namespace VSGE {
	class VulkanMesh : public Mesh {
	private:
		std::vector<VulkanBuffer*> vertexBuffers;
		VulkanBuffer* indexBuffer;
	public:

		VulkanMesh();
		VulkanMesh(const VulkanMesh&) = delete;

		virtual ~VulkanMesh();

		VulkanBuffer** GetVertexBuffers();
		VulkanBuffer* GetIndexBuffer();

		VkBuffer GetVertexBufferVk(uint32 i = 0);
		VkBuffer GetIndexBufferVk();

		bool Create();
		void Destroy();
	};
}